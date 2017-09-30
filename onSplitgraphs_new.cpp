#include "graph.hpp"

#include <vector>
#include <algorithm>
#include <utility>
#include "part_solution.cpp"

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
	//const int  & max1    = *ittomax; 								//maximum
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
			const int & n = adjlist_s[0];
			solution.push_back(n);
			Vertex & v = g.at(n);
			v.protect();
			return solution;
		}

		//otherwise

		//find the set Y of vertices y in I, for which N(y) intersection N(s) = emptyset
		//and for every z in N(s) the set X(z) of vertices x in I, for which N(x) intersection N(s) = {z}
		vec Y;
		std::unordered_multimap<int, int> X;

		for (int i=delimiter; i<g.size(); ++i) {
			Vertex & v   = g.at(i);
			vec    & adj = v.getAdjlist();
			std::vector<int> intersec;
			std::set_intersection(adj.begin(), adj.end(),
				                  adjlist_s.begin(), adjlist_s.end(),
				                  std::back_inserter(intersec));
			if(intersec.size() == 0) {
				Y.push_back(i);
			}
			else if(intersec.size() == 1) {
				const int & z = intersec[0];
				X.emplace(z, i);
			}
		}

        //case 1)
		if(Y.size() > 0) {
			//find y in Y with maximum degree
            int max_deg = 0;
            int max_y;
            // TODO: replace with std::max_element
            for(int i=0; i<Y.size(); ++i) {
                const int & n = Y[i];
                Vertex & y = g.at(n);
                if(y.numlinks() > max_deg) {
                    max_deg = y.numlinks();
                    max_y   = n;
                }
            }
            if(max_deg >= 3) {
                //compute u, v and add y
                solution = part_solution(g, pos, uc);
                solution.push_back(max_y);
            }

            //subcases
            else if(max_deg == 2) {
                Vertex & y     = g.at(max_y);
                vec    & adj_y = y.getAdjlist();
                for(int i=0; i<Y.size(); ++i) {
                    const int & n     = Y[i];
                    Vertex    & w     = g.at(n);
                    vec       & adj_w = w.getAdjlist();

                    //1a)
                    if(!std::includes(adj_y.begin(), adj_y.end(),
                                      adj_w.begin(), adj_w.end())){
                        //compute u, v and add y or w
                        solution = part_solution(g, pos, uc);
                        solution.push_back(max_y);
                    }

                    //1b)
                    else{
                        const int & p = adj_y[0];
                        const int & q = adj_y[1];
                        //subtract 1 from |U(p,q)|
                        pvec penalty;
                        penalty.emplace_back(p,q);

                        solution = part_solution(g, pos, uc, penalty);
                        //add y to the solution
                        solution.push_back(max_y);
                    }
                }
            }
            // 1c,1d
            else if(max_deg == 1) {
                Vertex & y     = g.at(max_y);
                vec    & adj_y = y.getAdjlist();
                vec      distinct;
                for(int i=0; i<Y.size(); ++i) {
                    const int & n     = Y[i];
                    Vertex    & w     = g.at(n);
                    vec       & adj_w = w.getAdjlist();
                    if(adj_y[0] != adj_w[0]) {
                        distinct.push_back(n);
                    }
                    if(distinct.size() > 2) {
                        break;
                    }
                }

                //1c)
                if(distinct.size() > 1) {
                    //compute u, v and add y
                    solution = part_solution(g, pos, uc);
                    solution.push_back(max_y);
                }

                //1d)
                else if(distinct.size() == 1) {
                    const int & p     = adj_y[0];
                    Vertex    & v     = g.at(distinct[0]);
                    const int & q     = v.getAdjlist()[0];
                    //subtract 1 from |U(p,q)|
                    pvec penalty;
                    penalty.emplace_back(p,q);
                    solution = part_solution(g, pos, uc, penalty);
                    //add y to the solution
                    //possible, that y is already saved, as (p,q) can be the best solution despite the penalty.
                    solution.push_back(max_y);
                }

                //1e) -> pretend to be in case 3)
                else{
                    pvec penalty;
                    // avoid multiple copying of adj list
                    vec union_vec(adj_y);
                    union_vec.emplace_back(0);
                    for(const auto & n : adjlist_s){
                        bool cont = true;
                        const auto xz = X.equal_range(n);

                        if(xz.first == X.end()) {
                            penalty.emplace_back(n, adj_y[0]);
                            continue;
                        }

                        union_vec[union_vec.size()-1] = n;
                        for(auto it = xz.first; it != xz.second; ++it) {
                            const int    & n     = it->second;
                            const Vertex & v     = g.at(n);
                            const vec    & adj_v = v.getAdjlist();
                            if(!std::includes(union_vec.begin(), union_vec.end(),
                                              adj_v.begin(), adj_v.end())){
                                cont = false;
                                break;
                            }
                        }
                        if(cont) {
                            penalty.emplace_back(n, adj_y[0]);
                        }
                    }
                    
                    solution = part_solution(g, pos, uc, penalty);
                    //third vertex to protect?
                }
            }
		}

        //case 2)
        else{
            // after running the algorithm, all free positions in xz are -1,
            // occupied positions have a value != -1
            vec xz;
            xz.reserve(adjlist_s.size());
            pvec penalty;
            for(int i=0; i<adjlist_s.size(); ++i) {
                bool cont     = true;
                const auto Xz = X.equal_range(adjlist_s[i]);
                int max_deg   = 0;
                int max_x     = -1;
                // TODO: max_element
                for(auto it = Xz.first; it != Xz.second; ++it) {
                    const int    & n = it->second;
                    const Vertex & x = g.at(n);
                    if(x.numlinks() > max_deg) {
                        max_deg = x.numlinks();
                        max_x   = n;
                    }
                }
                xz.push_back(max_x);
                const Vertex & max = g.at(max_x);
                if(max.numlinks() == 2) {
                    const auto & adj_max = max.getAdjlist();
                    for(auto it = Xz.first; it != Xz.second; ++it) {
                        const auto & v     = g.at(it->second);
                        const auto & adj_v = v.getAdjlist();
                        if(!std::includes(adj_max.begin(), adj_max.end(),
                                              adj_v.begin(), adj_v.end())) {
                            cont = false;
                            break;
                        }
                    }
                    if(cont) {
                        // add pair (p,q)
                        penalty.emplace_back(adj_max[0], adj_max[1]);
                    }
                }
            }
            
            vec bonus;
            for (int i=0; i<xz.size(); ++i) {
                if(xz[i] != -1) {
                    // position is occupied
                    bonus.push_back(adjlist_s[i]);
                }
            }
            solution = part_solution(g, pos, uc, penalty, bonus);
            //third vertex to protect?
        }

	}
	return solution;
}
