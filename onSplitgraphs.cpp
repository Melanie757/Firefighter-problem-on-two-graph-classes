#include "graph.hpp"
#include <vector>
#include <algorithm>
#include <utility>

using vec = std::vector<int>;

//input: splitgraph and position of the burning vertex in graph_
vec onSplitgraphs(Splitgraph & g, int pos) {
	vec solution;
	const int & delimiter = g.getDelimiter();

	//burn s and get the adjacencylist
	Vertex & s = g[pos];
	s.burn();
	vec & adjlist_s = s.getAdjlist();

	//vector with number of degree 1 neighbours for every vertex in clique:
	//look at every vertex of the independent set and if the ith vertex has 
	//only one neighbour increase the ith entry of uc by 1
	vec uc(delimiter);
	for (size_t i=delimiter; i<g.size(); ++i) {
		const Vertex & ind = g[i];
		const vec    & adj = ind.getAdjlist();
		if (adj.size() == 1) {
			const int & n = adj[0];
			++uc[n];
		}
	}

	//find vertex with maximal number of degree 1 neighbours
	//if no vertex has degree 1 neighbours: returns vertex with index 0
	const auto & ittomax = std::max_element(uc.begin(), uc.end()); 	//iterator on the maximum of uc
	const int  & max1    = *ittomax; 								//maximum
	const int  & index1  = std::distance(uc.begin(), ittomax); 		//index of the maximum in uc

//--------------------------------------------------------------------------------------------------------

	//first case: burning vertex is in clique
	if (pos < delimiter) {

		//vertex with maximal number of degree 1 neighbours is not s
		//=> protect this vertex
		if (pos != index1) {
			Vertex & sol1 = g[index1];
			sol1.protect();
			solution.push_back(index1);
		}

		//vertex with maximal number of degree 1 neighbours is s
		//=> find "second" maximum
		else if (pos == index1) {
			int max2   = 0;
			int index2 = 0;

			for (size_t i=0; i<uc.size(); ++i) {
				if (i == static_cast<size_t>(pos)) {
					continue;
				}
				int n = uc[i];
				if (n >= max2) {
					max2   = n;
					index2 = i;
				}
			}

			//protect the vertex found above
			Vertex & sol1 = g[index2];
			sol1.protect();
			solution.push_back(index2);
		}

		//find all degree one neighbours of the protected vertex and label them as saved
		const Vertex & sol1         = g[solution[0]];
		const vec    & adjlist_sol1 = sol1.getAdjlist();

		
		for(size_t i=0; i<adjlist_sol1.size(); ++i) {
			Vertex    & v         = g[adjlist_sol1[i]];
			const vec & adjlist_v = v.getAdjlist();
			if(adjlist_v.size() == 1) {
				v.saved();
			}
		}

		//burn unprotected neighbours of s
		for (size_t i=0; i<adjlist_s.size(); ++i) {
			Vertex & next = g[adjlist_s[i]];
			if (next.getState() == 0) {
				next.burn();
			}
		}

		//find an unprotected and not saved vertex in the independent set if it exists
		for (size_t i=delimiter; i<g.size(); ++i) {
			Vertex & sol2 = g[i];
			if (sol2.getState() == 0) {
				sol2.protect();
				solution.push_back(i);
				break;
			}
		}

		return solution;
	}

	//---------------------------------------------------------------------------------------------------

	//second case: burning vertex is in independent set
	if (pos >= delimiter) {

		//the burning vertex has degree 1
		//=> protect the neighbour so the fire cannot spread
		if (adjlist_s.size() == 1) {
			const int & n = adjlist_s[0];
			solution.push_back(n);
			Vertex & v = g[n];
			v.protect();
			return solution;
		}

		//otherwise

		//compute pair-bonus
    std::unordered_map<int, int> pairbonus;
//		vec pairbonus(del2); 	//The first delimiter entries create the bucket of the first vertex in C and so on.
							 	//If now for example vertex 4 is in the bucket of the first vertex,
								//the fourth entry of the vector is one.
//		std::vector<std::vector<int> > independent(del2); //stores the vertices of I related to the pairs with a non-zero pairbonus
		std::unordered_multimap<int, int> independent; //stores the vertices of I related to the pairs with a non-zero pairbonus

		//look at every vertex of the independent set
		for (size_t i=delimiter; i<g.size(); ++i) {
			//skip the burning vertex, because two neighbours of s cannot be protected
			if (i == static_cast<size_t>(pos)) {
				continue;
			}
			const Vertex  & ind = g[i];
			const vec     & adj = ind.getAdjlist();

			//test whether the vertex has degree 2
			if (adj.size() == 2) {

				//get the position of the two neighbours in graph_
				const int & c1 = adj[0];
				const int & c2 = adj[1];

				//add c_1 to the bucket of c_2 and vice versa
				++pairbonus[(delimiter * c1) + c2];
				++pairbonus[(delimiter * c2) + c1];

				//store the related vertices of the independent set
				independent.emplace(std::make_pair((delimiter * c1) + c2, i));
				independent.emplace(std::make_pair((delimiter * c2) + c1, i));
			}
		}

		/*
		for(int i=0; i<del2; ++i) {
			std::cout << i <<": ";
			vec n = independent[i];
			for(int j=0; j<n.size(); ++j) {
				std::cout << n[j] << ", ";
			}
			std::cout << std::endl;
		}
		*/

		/*
		std::cout << "pair-bonus: " << std::endl;
		for(int i=0; i<del2; ++i) {
			if(i%10 == 0) {std::cout << std::endl;}
			std::cout << pairbonus[i] << " ";
		}
		std::cout << std::endl;
		*/
		
		//calculate |U({c1,c2})| for every pair {c1,c2} in pairs and find the maximum
		//|U({c1,c2})| = pair-bonus + |U(c1)| + |U(c2)|
	
//		vec u2(del2); //vector to store |U({c1,c2})|
    std::unordered_map<int, int> u2;

    for (const auto & it : pairbonus){
			//find the pair of vertices related to pairbonus.at(i)
			const int c1 = it.first % delimiter;
			const int c2 = it.first / delimiter;

			/*
			std::cout << "c_1, c_2: " << c1 << ", " << c2 << std::endl;
			for(size_t i=0; i<adjlist_s.size(); ++i) {std::cout << adjlist_s.at(i) << ", ";}
			std::cout << std::endl;

			std::cout << !std::binary_search(adjlist_s.begin(), adjlist_s.end(), c1) << std::endl;
			std::cout << !std::binary_search(adjlist_s.begin(), adjlist_s.end(), c2) << std::endl;
			*/

			//if c1 is not adjacent to s, this pair of vertices can be a valid solution
			if (!std::binary_search(adjlist_s.begin(), adjlist_s.end(), c1)) {
				const int & pb = it.second;
				const int & n = uc[c1] + uc[c2];  //|U(c1)| + |U(c2)|
				u2[it.first] = pb + n;
				//std::cout << u2.at(i) << std::endl;
			}
			//else c2 cannot be a neighbour of s, since then c1 and c2 cannot be both protected
			else {
				if (!std::binary_search(adjlist_s.begin(), adjlist_s.end(), c2)) {
					const int & pb = it.second;
					const int & n = uc[c1] + uc[c2];  //|U(c1)| + |U(c2)|
					u2[it.first] = pb + n;
					//std::cout << u2.at(i) << std::endl;
				}
			}
	}

		/*
		std::cout << "u2: " << std::endl;
		for(int i=0; i<del2; ++i) {
			if(i%10 == 0) {std::cout << std::endl;}
			std::cout << u2[i] << " ";
		}
		std::cout << std::endl;
		*/

		//find the maximum of the |U({c1,c2})|
		int maxu2 = 0;
		int indexu2 = 0;

    if(u2.size() > 0){
		  const auto & ittomax = std::max_element(u2.begin(), u2.end(),
          [](const std::pair<int, int> & a,
             const std::pair<int, int> & b)
            {return a.second < b.second;}); 	//iterator on the maximum in u2
		  maxu2   = ittomax->second;												//maximum (>= 0)
		  indexu2 = ittomax->first;					//index of the maximum in u2
    }

		//find "second" maximum of uc
		//if every entry is 0: returns last index that is not pos or index1
		int max2   = 0;
		int index2 = 0;

		//if the vertex at index1 is not adjacent to s, it does not matter wheter the vertex at index2 is adjacent
		if (!std::binary_search(adjlist_s.begin(), adjlist_s.end(), index1)) {
			for (size_t i=0; i<uc.size(); ++i) {
				const int & n = uc[i];

				//index2 cannot be index1 => continue
				if (i == static_cast<size_t>(index1)) {
					continue;
				}

				if (n >= max2) {
					max2   = n;
					index2 = i;
				}
			}
		}

		//if index1 is adjacent to s, index2 cannot be a neighbour of s (otherwise both cannot be protected)
		else {
			for (size_t i=0; i<uc.size(); ++i) {
				//skip the entries in uc that belong to neighbours of s
				if (std::binary_search(adjlist_s.begin(), adjlist_s.end(), i)) {
					continue;
				}
				const int & n = uc[i];
				if (n >= max2) {
					max2   = n;
					index2 = i;
				}
			}
		}
		
		int maxuc = max1 + max2; //max{|U(c1)| + |U(c2)|} (>= 0, because |U(c1)|, |U(c2)| >= 0)

		//TODO label saved vertices!!!!!!!

		//if max{|U(c1)| + |U(c2)|} < max{|U({c1,c2})|}
		if (maxuc < maxu2) {
			//get the pair with max{|U({c1,c2})|}
			const int c1 = indexu2 % delimiter;
			const int c2 = indexu2 / delimiter;
			//save the pair in the solution vector
			solution.push_back(c1);
			solution.push_back(c2);

			Vertex & v1 = g[c1];
			Vertex & v2 = g[c2];
			v1.protect();
			v2.protect();

			//label degree one vertices that are saved by the vertices of the pair
			if (uc[c1] != 0) {
				vec & adj_1 = v1.getAdjlist();
				for (size_t i=0; i<adj_1.size(); ++i) {
					Vertex & n = g[adj_1[i]];
					const vec & adj_n = n.getAdjlist();
					if(adj_n.size() == 1) {
						n.saved();
					}
				}
			}

			if (uc[c2] != 0) {
				vec & adj_2 = v2.getAdjlist();
				for (size_t i=0; i<adj_2.size(); ++i) {
					Vertex & n = g[adj_2[i]];
					const vec & adj_n = n.getAdjlist();
					if(adj_n.size() == 1) {
						n.saved();
					}
				}
			}
			
			//label vertices that are saved by the pair
			const auto neighbour = independent.equal_range((delimiter * c1) + c2);
			for(auto it = neighbour.first; it != neighbour.second; ++it) {
				const int & n = it->second;
				Vertex & v = g[n];
				v.saved();
			}

			
			
		}

		//if max{|U(c1)| + |U(c2)|} >= max{|U({c1,c2})|}
		else if (maxuc >= maxu2) {
			solution.push_back(index1);
			solution.push_back(index2);
			
			//label saved vertices
			Vertex & v1 = g[index1];
			v1.protect();
			vec & adj_1 = v1.getAdjlist();
			for(size_t i=0; i<adj_1.size(); ++i) {
				Vertex    & v         = g[adj_1[i]];
				const vec & adj_v = v.getAdjlist();
				if(adj_v.size() == 1) {
					v.saved();
				}
			}
			Vertex & v2 = g[index2];
			v2.protect();
			vec & adj_2 = v2.getAdjlist();
			for(size_t i=0; i<adj_2.size(); ++i) {
				Vertex    & v         = g[adj_2[i]];
				const vec & adj_v = v.getAdjlist();
				if(adj_v.size() == 1) {
					v.saved();
				}
			}
		}
		

		//if max{|U(c1)| + |U(c2)|} = max{|U({c1,c2})|} = 0
		//=> no matter which vertex is protected, because it saves no other vertices
		else {
			solution.push_back(adjlist_s[0]);
			Vertex & sol = g[solution[0]];
			sol.protect();
			//burn unprotected vertices adjacent to s
			for (size_t i=0; i<adjlist_s.size(); ++i) {
				Vertex & next = g[adjlist_s[i]];
				if (next.getState() == 0) {next.burn();}
			}
			//find unburned vertex to protect if it exists
			for (size_t i=0; i<g.size(); ++i) {
				Vertex & v = g[i];
				if (v.getState() == 0) {
					solution.push_back(i);
					v.protect();
					break;
				}
			}
		}

		//check whether there exists another vertex to protect (in O(n^2))
		/*
		for(int i=0; i<adjlist_s.size(); ++i) {
			Vertex & v = g.at(adjlist_s[i]);
			if(v.getState() == 0) {
				v.burn();
				vec & adj = v.getAdjlist();
				for(int j=0; j < adj.size(); ++j) {
					const int & n = adj[j];
					//std::cout << n << std::endl;
					if(n >= delimiter) {
						Vertex & u = g[n];
						u.burn();
					}
				}
			}
		}

		for(int i=delimiter; i<g.size(); ++i) {
			Vertex & v = g[i];
			if(v.getState() == 0) {
				v.protect();
				solution.push_back(i);
				break;
			}
		}
		*/
		
		/*
		for(int i=0; i<g.size(); ++i) {
			Vertex & v = g[i];
			std::cout << "Vertex " << i << ": " << v.getState() << std::endl;
		}
		*/

		
		vec ok(g.size());
		ok[pos] = 1;

		for(size_t i=0; i<adjlist_s.size(); ++i) {
			ok[adjlist_s[i]] = 1;
		}
		for(size_t i=0; i<solution.size(); ++i) {
			ok[solution[i]] = 0;
		}


		for(size_t i=delimiter; i<ok.size(); ++i) {
			if(ok[i] == 0) {
				Vertex & v = g[i];
				const vec & adj = v.getAdjlist();
				for(size_t j=0; j<adj.size(); ++j) {
					//const int & n = adj[j];
					if(ok[adj[j]] == 1) {
						ok[i] = 1;
						break;
					}
				}
			}
		}

		/*
		for(int i=0; i<g.size(); ++i) {
			std::cout << "Vertex" << i << ": " << ok[i] << std::endl;;
		}
		std::cout << std::endl;
		*/

		for(size_t i=delimiter; i<g.size(); ++i) {
			if(ok[i] == 0) {
				Vertex & v = g[i];
				if(v.getState() == 0) {
					v.protect();
					solution.push_back(i);
					break;
				}
			}
		}
		
		

		/*
		for(int i=delimiter; i < g.size(); ++i) {
			Vertex & v = g[i];
			std::cout << "State Vertex " << i << ": " << v.getState() << std::endl;
		}
		*/

	}
	return solution;
}
