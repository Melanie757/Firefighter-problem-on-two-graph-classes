#include "graph.hpp"
#include "part_solution.hpp"
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;  
    }
};

using unordered_map_pair = std::unordered_map<std::pair<int, int>, int, pair_hash>;

//vec penalty: first and second entries are a pair, third and forth, ...
//vec bonus: single vertices
vec part_solution(Splitgraph & g, const int & pos, vec & adjlist_s, vec & uc, pvec penalty, vec bonus) { 
	vec solution;

  // for (int i=0; i<bonus.size(); ++i) {
  //   std::cout << bonus[i] << " ";
  // }
  // std::cout << std::endl;

  // for (int i=0; i< uc.size(); ++i) {
  //   std::cout<< uc[i] << " ";
  // }
  // std::cout << std::endl;

	for(int i=0; i<bonus.size(); ++i) {
		++uc[bonus[i]];
	}

  // for(int i=0; i< uc.size(); ++i) {
  //   std::cout<<uc[i] << " ";
  // }
  // std::cout << std::endl;

  //find the maximum in uc
	// const auto & ittomax = std::max_element(uc.begin(), uc.end()); 	//iterator on the maximum of uc
	// const int  & max1    = *ittomax; 							                 	//maximum
	// const int  & index1  = std::distance(uc.begin(), ittomax); 		  //index of the maximum in uc

 //  //find "second" maximum of uc
 //  //if every entry is 0: returns last index that is not pos or index1
 //  int max2   = 0;
 //  int index2 = 0;

 //  //if the vertex at index1 is not adjacent to s, it does not matter wheter the vertex at index2 is adjacent
 //  if (!std::binary_search(adjlist_s.begin(), adjlist_s.end(), index1)) {
 //    for (int i=0; i<uc.size(); ++i) {
 //      const int & n = uc[i];

 //      //index2 cannot be index1 => continue
 //      if (i == index1) {
 //        continue;
 //      }

 //      if (n >= max2) {
 //        max2   = n;
 //        index2 = i;
 //      }
 //    }
 //  }

  //if index1 is adjacent to s, index2 cannot be a neighbour of s (otherwise both cannot be protected)
  // else {
  //   for (int i=0; i<uc.size(); ++i) {
  //     //skip the entries in uc that belong to neighbours of s
  //     if (std::binary_search(adjlist_s.begin(), adjlist_s.end(), i)) {
  //       continue;
  //     }
  //     const int & n = uc.at(i);
  //     if (n >= max2) {
  //       max2   = n;
  //       index2 = i;
  //     }
  //   }
  // }


	//compute pair-bonus
	const int & delimiter = g.getDelimiter();

  unordered_map_pair pairbonus;

  std::unordered_multimap<int, int> independent;	

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

      auto p = std::minmax(c1, c2);

			//add c_1 to the bucket of c_2 and vice versa
			++ pairbonus[p];

			//store the related vertices of the independent set
			// independent[(delimiter * c1) + c2].push_back(i);
			// independent[(delimiter * c2) + c1].push_back(i);
			independent.emplace((delimiter * c1) + c2, i);
			independent.emplace((delimiter * c2) + c1, i);
		}
	}

  //penalty as negative pairbonus
  for(const auto & p : penalty) {
    --pairbonus[p];
  }
			
	//calculate |U({c1,c2})| for every pair {c1,c2} in pairs and find the maximum
	//|U({c1,c2})| = pair-bonus + |U(c1)| + |U(c2)|

	unordered_map_pair u2;

	//for (const auto & it : pairbonus) {
  for(int i=0; i<delimiter*delimiter; ++i){
		const int c1 = i / delimiter;
    const int c2 = i % delimiter;
		//find the pair of vertices related to pairbonus.at(i)
		//const int c1 = it.first.first;
		//const int c2 = it.first.second;

    if(c1 < c2) {
      const auto c12 = std::make_pair(c1,c2);
      int pb = 0;
      //if c1 is not adjacent to s, this pair of vertices can be a valid solution
      if (!std::binary_search(adjlist_s.begin(), adjlist_s.end(), c1)) {
        if(pairbonus.count(c12)){
          pb = pairbonus[c12];
        }
        //const int & n = uc[c1] + uc[c2];  //|U(c1)| + |U(c2)|
        u2[c12] = pb + uc[c1] + uc[c2];
      }

      //else c2 cannot be a neighbour of s, since then c1 and c2 cannot be both protected
      else {
        if (!std::binary_search(adjlist_s.begin(), adjlist_s.end(), c2)) {
          if(pairbonus.count(c12)){
            pb = pairbonus[c12];
          }
          //const int & n = uc[c1] + uc[c2];  //|U(c1)| + |U(c2)|
          u2[c12] = pb + uc[c1] + uc[c2];
        }
      }

    }

	}


  // std::cout << "u2: ";
  // for(const auto & pair : u2) {
  //   std::cout << "(" << pair.first.first << ", " << pair.first.second << ")" << " " << pair.second << std::endl;
  // }

  // std::cout << "penalty: ";
  // for(const auto & pair : penalty) {
  //   std::cout << "(" << pair.first << ", " << pair.second << ")" << std::endl;
  // }

	//find the maximum of the |U({c1,c2})|
	//int maxu = 0;
	//int indexu = 0;

	const auto & ittomaxu2 = std::max_element(u2.begin(), u2.end(),
    [](const std::pair<std::pair<int,int>, int> & a,
       const std::pair<std::pair<int,int>, int> & b)
        {return (a.second < b.second);});
	//int  maxu2  = ittomaxu2->second;			  //maximum (>= 0)
	auto pairu2 = ittomaxu2->first;		    //pair attaining the maximum

  const auto & c1 = pairu2.first;
  const auto & c2 = pairu2.second;

	solution.push_back(c1);
	solution.push_back(c2);

	Vertex & v1 = g.at(c1);
	Vertex & v2 = g.at(c2);
	v1.protect();
	v2.protect();

	//label degree one vertices that are saved by the vertices of the pair
	if (uc[c1] != 0) {
		vec & adj_1 = v1.getAdjlist();
		for (int i=0; i<adj_1.size(); ++i) {
			Vertex & n = g.at(adj_1[i]);
			vec & adj_n = n.getAdjlist();
			if(adj_n.size() == 1) {
				n.saved();
			}
		}
	}

	if (uc[c2] != 0) {
		vec & adj_2 = v2.getAdjlist();
		for (int i=0; i<adj_2.size(); ++i) {
			Vertex & n = g.at(adj_2[i]);
			vec & adj_n = n.getAdjlist();
			if(adj_n.size() == 1) {
				n.saved();
			}
		}
	}
				
	//label vertices that are saved by the pair
	const auto neighbour = independent.equal_range((delimiter * c1) + c2);
	for(auto it = neighbour.first; it!= neighbour.second; ++it) {
		const int & n = it->second;
		Vertex & v = g.at(n);
		v.saved();
	}			

	return solution;
}
