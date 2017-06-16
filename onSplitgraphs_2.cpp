#include <vector>
#include <algorithm>
#include <utility>

using vec = std::vector<int>;

//input: splitgraph and position of the burning vertex in graph_
vec onSplitgraphs(Splitgraph & g, int pos) {
	int saved = 0; //number of saved vertices
	vec solution;
	const int & delimiter = g.getDelimiter();

	//burn s and get the adjacencylist
	Vertex & s = g.at(pos);
	s.burn();
	vec & adjlist_s = s.getAdjlist();

	//vector with number of degree 1 neighbours for every vertex in clique:
	//look at every vertex of the independent set and if the ith vertex has 
	//only one neighbour increase the ith entry of uc by 1
	vec uc(delimiter);
	for (int i=delimiter; i<g.size(); ++i) {
		const Vertex & ind = g.at(i);
		const vec    & adj = ind.getAdjlist();
		if (adj.size() == 1) {
			const int & n = adj.at(0);
			++uc.at(n);
		}
	}

	//find vertex with maximal number of degree 1 neighbours
	//if no vertex has degree 1 neighbours: returns vertex with index 0
	const auto & ittomax = std::max_element(uc.begin(), uc.end()); 	//iterator on the maximum of uc
	const int  & max1    = *ittomax; 								//maximum
	const int  & index1  = std::distance(uc.begin(), ittomax); 		//index of the maximum in uc



	//first case: burning vertex is in clique
	if (pos < delimiter) {

		//vertex with maximal number of degree 1 neighbours is not s
		//=> protect this vertex
		if (pos != index1) {
			Vertex & sol1 = g.at(index1);
			sol1.protect();
			solution.push_back(index1);
			saved += max1 + 1;
		}

		//vertex with maximal number of degree 1 neighbours is s
		//=> find "second" maximum
		else if (pos == index1) {
			int max2   = 0;
			int index2 = 0;

			for (int i=0; i<uc.size(); ++i) {
				if (i == pos) {
					continue;
				}
				int n = uc.at(i);
				if (n >= max2) {
					max2   = n;
					index2 = i;
				}
			}

			//protect the vertex found above
			Vertex & sol1 = g.at(index2);
			sol1.protect();
			solution.push_back(index2);
			saved += max2 + 1;
		}

		//find all degree one neighbours of the protected vertex and label them as saved
		const Vertex & sol1         = g.at(solution.at(0));
		const vec    & adjlist_sol1 = sol1.getAdjlist();

		#pragma omp parallel
		{
			#pragma omp for schedule(static)
			for(int i=0; i<adjlist_sol1.size(); ++i) {
				Vertex    & v         = g.at(adjlist_sol1.at(i));
				const vec & adjlist_v = v.getAdjlist();
				if(adjlist_v.size() == 1) {
					v.saved();
				}
			}

			//burn unprotected neighbours of s
			#pragma omp for schedule(static)
			for (int i=0; i<adjlist_s.size(); ++i) {
				Vertex & next = g.at(adjlist_s.at(i));
				if (next.getState() == 0) {
					next.burn();
				}
			}
		}

		//find an unprotected and not saved vertex in the independent set if it exists
		for (int i=delimiter; i<g.size(); ++i) {
			Vertex & sol2 = g.at(i);
			if (sol2.getState() == 0) {
				sol2.protect();
				solution.push_back(i);
				saved += 1;
				break;
			}
		}

		/*
		//burn vertices in the last time step
		for (int i=delimiter; i<g.size(); ++i) {
			Vertex & v = g.at(i);
			if(v.getState() == 0) {v.burn();}
		}
		*/
		return solution;
	}

	//---------------------------------------------------------------------------------------------------

	//second case: burning vertex is in independent set
	if (pos >= delimiter) {

		//the burning vertex has degree 1
		//=> protect the neighbour so the fire cannot spread
		if (adjlist_s.size() == 1) {
			const int & n = adjlist_s.at(0);
			solution.push_back(n);
			Vertex & v = g.at(n);
			v.protect();
			return solution;
		}

		//otherwise

		//compute pair-bonus
		std::vector<std::pair<int,int> > pairs; //stores pairs with nonzero pair-bonus
		vec pb; //stores pair-bonus of every pair in pairs

		//look at every vertex of the independent set
		for (int i=delimiter; i<g.size(); ++i) {
			//skip the burning vertex, because two neighbours of s cannot be protected
			if (i == pos) {
				continue;
			}
			const Vertex  & ind = g.at(i);
			const vec     & adj = ind.getAdjlist();

			//test whether the vertex has degree 2
			if (adj.size() == 2) {

				//get the position of the two neighbours in graph_
				const int & c1 = adj.at(0);
				const int & c2 = adj.at(1);

				//make the two pairs possible with c1 and c2
				const std::pair<int,int> & p1 = std::make_pair(c1,c2);
				const std::pair<int,int> & p2 = std::make_pair(c2,c1);

				//get iterators on p1 and p2 in the vector pairs
				const auto & pos1 = std::find(pairs.begin(), pairs.end(), p1);
				const auto & pos2 = std::find(pairs.begin(), pairs.end(), p2);

				//if the pairs cannot be found in the vector pairs
				//=> store p1 in pairs and create the belonging entry in pb with 1
				if ( (pos1 == pairs.end()) && (pos2 == pairs.end()) ) {
					pairs.push_back(p1);
					pb.push_back(1);
				}

				//if p1 is already in pairs increase the belonging entry in pb by 1
				else if (pos1 != pairs.end()) {
					const int & pos = std::distance(pairs.begin(), pos1);
					++pb.at(pos);
				}

				//if p2 is already in pairs increase the belonging entry in pb by 1
				else if (pos2 != pairs.end()) {
					const int & pos = std::distance(pairs.begin(), pos2);
					++pb.at(pos);
				}
				
			}
		}
		
		//calculate |U({c1,c2})| for every pair {c1,c2} in pairs and find the maximum
		//|U({c1,c2})| = pair-bonus + |U(c1)| + |U(c2)|
		int maxu2 = 0;
		int index = 0;

		//if no pairs were found set the maximum to -1
		if (pairs.size() == 0) {
			maxu2 = -1;
		}

		else {
			vec u2(pairs.size()); //vector to store |U({c1,c2})|
			for (int i=0; i<pairs.size(); ++i) {

				//get the entries of the current pair
				const auto & pair = pairs.at(i);
				const int  & c1 = std::get<0>(pair);
				const int  & c2 = std::get<1>(pair);

				//get iterator on their position in the adjacencylist of s
				const auto & n1 = std::lower_bound(adjlist_s.begin(), adjlist_s.end(), c1);
				const auto & n2 = std::lower_bound(adjlist_s.begin(), adjlist_s.end(), c2); 

				//if both vertices are adjacent to s: pair cannot be protected
				if ( (n1 != adjlist_s.end()) && (n2 != adjlist_s.end()) ) {
					u2.at(i) = -1; //set |U({c1,c2})| to -1
					continue;
				}

				//if not both vertices are adjacent to s: compute |U({c1,c2})|
				const int & n = uc.at(c1) + uc.at(c2);  //|U(c1)| + |U(c2)|
				const int & m = pb.at(i);			 	//pair-bonus
				u2.at(i) = m+n;							//pair-bonus + |U(c1)| + |U(c2)|
			}

			//find the maximum of the |U({c1,c2})|
			const auto & ittomax = std::max_element(u2.begin(), u2.end()); 	//iterator on the maximum in u2
			maxu2 = *ittomax;												//maximum
			index = std::distance(u2.begin(), ittomax);						//index of the maximum in u2
		}
		

		//find "second" maximum of uc
		//if every entry is 0: returns last index thats not pos or index1
		int max2   = 0;
		int index2 = 0;

		//if index1 is adjacent to s, then index2 cannot be a neighbour of s (otherwise it cannot be protected)
		if (std::lower_bound(adjlist_s.begin(), adjlist_s.end(), index1) != adjlist_s.end()) {
			for (int i=0; i<uc.size(); ++i) {
				//skip the entries in uc that belong to neighbours of s
				if (std::lower_bound(adjlist_s.begin(), adjlist_s.end(), i) != adjlist_s.end()) {
					continue;
				}
				const int & n = uc.at(i);
				if (n >= max2) {
					max2   = n;
					index2 = i;
				}
			}
		}

		//if index1 is not adjacent to s, then index2 can be adjacent to s
		else {
			for (int i=0; i<uc.size(); ++i) {
				const int & n = uc.at(i);

				//index2 cannot be index1 => continue
				if (i == index1) {
					continue;
				}

				if (n >= max2) {
					max2   = n;
					index2 = i;
				}
			}

		}
		
		int maxuc = max1 + max2; //max{|U(c1)| + |U(c2)|} (>= 0, because |U(c1)|, |U(c2)| >= 0)

		//if max{|U(c1)| + |U(c2)|} < max{|U({c1,c2})|} (=> max{|U({c1,c2})|} > 0)
		if (maxuc < maxu2) {
			//get the pair with max{|U({c1,c2})|}
			const auto & pair = pairs.at(index);
			//save the pair in the solution vector
			solution.push_back(std::get<0>(pair));
			solution.push_back(std::get<1>(pair));
			saved += maxu2 + 2;
		}

		//if max{|U(c1)| + |U(c2)|} >= max{|U({c1,c2})|} and max{|U(c1)| + |U(c2)|} > 0
		else if ( (maxuc >= maxu2) && (maxuc > 0) ) {
			solution.push_back(index1);
			solution.push_back(index2);
			saved += maxuc + 2;
		}

		//if max{|U(c1)| + |U(c2)|} >= max{|U({c1,c2})|} and max{|U(c1)| + |U(c2)|} = 0
		//(possible, because max{|U({c1,c2})|} can be -1)
		//=> no matter which vertex is protected, because it saves no other vertices
		else {
			solution.push_back(adjlist_s.at(0));
			Vertex & sol = g.at(solution.at(0));
			sol.protect();
			saved += 1;
			//burn unprotected vertices adjacent to s
			for (int i=0; i<adjlist_s.size(); ++i) {
				Vertex & next = g.at(adjlist_s.at(i));
				if (next.getState() == 0) {next.burn();}
			}
			//find unburned vertex to protect if it exists
			for (int i=0; i<g.size(); ++i) {
				Vertex & v = g.at(i);
				if (v.getState() == 0) {
					solution.push_back(i);
					v.protect();
					saved += 1;
					break;
				}
			}
		}

	}
	return solution;
}