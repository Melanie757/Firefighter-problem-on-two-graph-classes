#include <vector>
#include <algorithm>
#include <utility>

using vec = std::vector<int>;

//input: splitgraph and position of the burning vertex in graph_
vec onSplitgraphs(Splitgraph & g, int pos) {
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

//--------------------------------------------------------------------------------------------------------

	//first case: burning vertex is in clique
	if (pos < delimiter) {

		//vertex with maximal number of degree 1 neighbours is not s
		//=> protect this vertex
		if (pos != index1) {
			Vertex & sol1 = g.at(index1);
			sol1.protect();
			solution.push_back(index1);
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
		}

		//find all degree one neighbours of the protected vertex and label them as saved
		const Vertex & sol1         = g.at(solution.at(0));
		const vec    & adjlist_sol1 = sol1.getAdjlist();

		
		for(int i=0; i<adjlist_sol1.size(); ++i) {
			Vertex    & v         = g.at(adjlist_sol1.at(i));
			const vec & adjlist_v = v.getAdjlist();
			if(adjlist_v.size() == 1) {
				v.saved();
			}
		}

		//burn unprotected neighbours of s
		for (int i=0; i<adjlist_s.size(); ++i) {
			Vertex & next = g.at(adjlist_s.at(i));
			if (next.getState() == 0) {
				next.burn();
			}
		}

		//find an unprotected and not saved vertex in the independent set if it exists
		for (int i=delimiter; i<g.size(); ++i) {
			Vertex & sol2 = g.at(i);
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
			const int & n = adjlist_s.at(0);
			solution.push_back(n);
			Vertex & v = g.at(n);
			v.protect();
			return solution;
		}

		//otherwise

		//compute pair-bonus
		const int del2 = delimiter * delimiter;
		vec pairbonus(del2); 	//The first delimiter entries create the bucket of the first vertex in C and so on.
							 	//If now for example vertex 4 is in the bucket of the first vertex,
								//the fourth entry of the vector is one.
		std::vector<std::vector<int> > independent(del2); //stores the vertices of I related to the pairs with a non-zero pairbonus

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

				//add c_1 to the bucket of c_2 and vice versa
				++ pairbonus.at((delimiter * c1) + c2);
				++ pairbonus.at((delimiter * c2) + c1);

				//store the related vertices of the independent set
				independent[(delimiter * c1) + c2].push_back(i);
				independent[(delimiter * c2) + c1].push_back(i);
			}
		}

		/*
		for(int i=0; i<del2; ++i) {
			std::cout << i <<": ";
			vec n = independent.at(i);
			for(int j=0; j<n.size(); ++j) {
				std::cout << n.at(j) << ", ";
			}
			std::cout << std::endl;
		}
		*/

		/*
		std::cout << "pair-bonus: " << std::endl;
		for(int i=0; i<del2; ++i) {
			if(i%10 == 0) {std::cout << std::endl;}
			std::cout << pairbonus.at(i) << " ";
		}
		std::cout << std::endl;
		*/
		
		//calculate |U({c1,c2})| for every pair {c1,c2} in pairs and find the maximum
		//|U({c1,c2})| = pair-bonus + |U(c1)| + |U(c2)|
	
		vec u2(del2); //vector to store |U({c1,c2})|

		for (int i=0; i<del2; ++i) {
			if (pairbonus.at(i) == 0) {
				continue;
			}
			else {
				//find the pair of vertices related to pairbonus.at(i)
				const int c1 = i % delimiter;
				const int c2 = i / delimiter;

				/*
				std::cout << "c_1, c_2: " << c1 << ", " << c2 << std::endl;
				for(int i=0; i<adjlist_s.size(); ++i) {std::cout << adjlist_s.at(i) << ", ";}
				std::cout << std::endl;

				std::cout << !std::binary_search(adjlist_s.begin(), adjlist_s.end(), c1) << std::endl;
				std::cout << !std::binary_search(adjlist_s.begin(), adjlist_s.end(), c2) << std::endl;
				*/

				//if c1 is not adjacent to s, this pair of vertices can be a valid solution
				if (!std::binary_search(adjlist_s.begin(), adjlist_s.end(), c1)) {
					const int & pb = pairbonus.at(i);
					const int & n = uc.at(c1) + uc.at(c2);  //|U(c1)| + |U(c2)|
					u2.at(i) = pb + n;
					//std::cout << u2.at(i) << std::endl;
				}
				//else c2 cannot be a neighbour of s, since then c1 and c2 cannot be both protected
				else {
					if (!std::binary_search(adjlist_s.begin(), adjlist_s.end(), c2)) {
						const int & pb = pairbonus.at(i);
						const int & n = uc.at(c1) + uc.at(c2);  //|U(c1)| + |U(c2)|
						u2.at(i) = pb + n;
						//std::cout << u2.at(i) << std::endl;
					}
					else {
						continue;
					}
				}

			}
		}

		/*
		std::cout << "u2: " << std::endl;
		for(int i=0; i<del2; ++i) {
			if(i%10 == 0) {std::cout << std::endl;}
			std::cout << u2.at(i) << " ";
		}
		std::cout << std::endl;
		*/

		//find the maximum of the |U({c1,c2})|
		int maxu2 = 0;
		int indexu2 = 0;

		const auto & ittomax = std::max_element(u2.begin(), u2.end()); 	//iterator on the maximum in u2
		maxu2 = *ittomax;												//maximum (>= 0)
		indexu2 = std::distance(u2.begin(), ittomax);					//index of the maximum in u2
		

		//find "second" maximum of uc
		//if every entry is 0: returns last index that is not pos or index1
		int max2   = 0;
		int index2 = 0;

		//if the vertex at index1 is not adjacent to s, it does not matter wheter the vertex at index2 is adjacent
		if (!std::binary_search(adjlist_s.begin(), adjlist_s.end(), index1)) {
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

		//if index1 is adjacent to s, index2 cannot be a neighbour of s (otherwise both cannot be protected)
		else {
			for (int i=0; i<uc.size(); ++i) {
				//skip the entries in uc that belong to neighbours of s
				if (std::binary_search(adjlist_s.begin(), adjlist_s.end(), i)) {
					continue;
				}
				const int & n = uc.at(i);
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

			Vertex & v1 = g.at(c1);
			Vertex & v2 = g.at(c2);
			v1.protect();
			v2.protect();

			//label degree one vertices that are saved by the vertices of the pair
			if (uc.at(c1) != 0) {
				vec & adj_1 = v1.getAdjlist();
				for (int i=0; i<adj_1.size(); ++i) {
					Vertex & n = g.at(adj_1.at(i));
					vec & adj_n = n.getAdjlist();
					if(adj_n.size() == 1) {
						n.saved();
					}
				}
			}

			if (uc.at(c2) != 0) {
				vec & adj_2 = v2.getAdjlist();
				for (int i=0; i<adj_2.size(); ++i) {
					Vertex & n = g.at(adj_2.at(i));
					vec & adj_n = n.getAdjlist();
					if(adj_n.size() == 1) {
						n.saved();
					}
				}
			}
			
			//label vertices that are saved by the pair
			vec neighbour = independent.at((delimiter * c1) + c2);
			for(int i=0; i<neighbour.size(); ++i) {
				const int & n = neighbour.at(i);
				Vertex & v = g.at(n);
				v.saved();
			}

			
			
		}

		//if max{|U(c1)| + |U(c2)|} >= max{|U({c1,c2})|}
		else if (maxuc >= maxu2) {
			solution.push_back(index1);
			solution.push_back(index2);
			
			//label saved vertices
			Vertex & v1 = g.at(index1);
			v1.protect();
			vec & adj_1 = v1.getAdjlist();
			for(int i=0; i<adj_1.size(); ++i) {
				Vertex    & v         = g.at(adj_1.at(i));
				const vec & adj_v = v.getAdjlist();
				if(adj_v.size() == 1) {
					v.saved();
				}
			}
			Vertex & v2 = g.at(index2);
			v2.protect();
			vec & adj_2 = v2.getAdjlist();
			for(int i=0; i<adj_2.size(); ++i) {
				Vertex    & v         = g.at(adj_2.at(i));
				const vec & adj_v = v.getAdjlist();
				if(adj_v.size() == 1) {
					v.saved();
				}
			}
		}
		

		//if max{|U(c1)| + |U(c2)|} = max{|U({c1,c2})|} = 0
		//=> no matter which vertex is protected, because it saves no other vertices
		else {
			solution.push_back(adjlist_s.at(0));
			Vertex & sol = g.at(solution.at(0));
			sol.protect();
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
					break;
				}
			}
		}

		//check whether there exists another vertex to protect (in O(n^2))
		/*
		for(int i=0; i<adjlist_s.size(); ++i) {
			Vertex & v = g.at(adjlist_s.at(i));
			if(v.getState() == 0) {
				v.burn();
				vec & adj = v.getAdjlist();
				for(int j=0; j < adj.size(); ++j) {
					const int & n = adj.at(j);
					//std::cout << n << std::endl;
					if(n >= delimiter) {
						Vertex & u = g.at(n);
						u.burn();
					}
				}
			}
		}

		for(int i=delimiter; i<g.size(); ++i) {
			Vertex & v = g.at(i);
			if(v.getState() == 0) {
				v.protect();
				solution.push_back(i);
				break;
			}
		}
		*/
		
		/*
		for(int i=0; i<g.size(); ++i) {
			Vertex & v = g.at(i);
			std::cout << "Vertex " << i << ": " << v.getState() << std::endl;
		}
		*/

		
		vec ok(g.size());
		ok.at(pos) = 1;

		for(int i=0; i<adjlist_s.size(); ++i) {
			ok.at(adjlist_s.at(i)) = 1;
		}
		for(int i=0; i<solution.size(); ++i) {
			ok.at(solution.at(i)) = 0;
		}


		for(int i=delimiter; i<ok.size(); ++i) {
			if(ok.at(i) == 0) {
				Vertex & v = g.at(i);
				const vec & adj = v.getAdjlist();
				for(int j=0; j<adj.size(); ++j) {
					//const int & n = adj.at(j);
					if(ok.at(adj.at(j)) == 1) {
						ok.at(i) = 1;
						break;
					}
				}
			}
		}

		/*
		for(int i=0; i<g.size(); ++i) {
			std::cout << "Vertex" << i << ": " << ok.at(i) << std::endl;;
		}
		std::cout << std::endl;
		*/

		for(int i=delimiter; i<g.size(); ++i) {
			if(ok.at(i) == 0) {
				Vertex & v = g.at(i);
				if(v.getState() == 0) {
					v.protect();
					solution.push_back(i);
					break;
				}
			}
		}
		
		

		/*
		for(int i=delimiter; i < g.size(); ++i) {
			Vertex & v = g.at(i);
			std::cout << "State Vertex " << i << ": " << v.getState() << std::endl;
		}
		*/

	}
	return solution;
}