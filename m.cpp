#include <iostream>
#include <unordered_map>  // C++11 feature: unordered_map
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>      // For STL heap operations
#include <cctype>         // For toupper
#include <climits>        // For INT_MAX
#include <cmath>          // For ceil and pow

using namespace std;

class Graph_M {
private:
    struct Vertex {
        unordered_map<string, int> nbrs;  // C++11: unordered_map for O(1) average lookup
    };

    unordered_map<string, Vertex> vtces;  // Main graph storage

    struct DijkstraPair {
        string vname;
        string psf;
        int cost;

        // C++11: operator< for STL heap (min-heap behavior with max-heap implementation)
        bool operator<(const DijkstraPair& other) const {
            return cost > other.cost;  // Reverse for min-heap
        }
    };

    struct Pair {
        string vname;
        string psf;
        int min_dis;
        int min_time;
    };

public:
    Graph_M() {}

    int numVertex() {
        return vtces.size();
    }

    bool containsVertex(const string& vname) {
        return vtces.find(vname) != vtces.end();
    }

    void addVertex(const string& vname) {
        Vertex vtx;
        vtces[vname] = vtx;
    }

    void removeVertex(const string& vname) {
        Vertex& vtx = vtces[vname];
        vector<string> keys;
        for (const auto& pair : vtx.nbrs) {  // C++11: auto keyword
            keys.push_back(pair.first);
        }
        for (const string& key : keys) {     // C++11: range-based for
            vtces[key].nbrs.erase(vname);
        }
        vtces.erase(vname);
    }

    int numEdges() {
        int count = 0;
        for (const auto& pair : vtces) {     // C++11: auto and range-based for
            count += pair.second.nbrs.size();
        }
        return count / 2;
    }

    bool containsEdge(const string& vname1, const string& vname2) {
        if (vtces.find(vname1) == vtces.end() || vtces.find(vname2) == vtces.end()) {
            return false;
        }
        return vtces[vname1].nbrs.find(vname2) != vtces[vname1].nbrs.end();
    }

    void addEdge(const string& vname1, const string& vname2, int value) {
        if (vtces.find(vname1) == vtces.end() || vtces.find(vname2) == vtces.end() || 
            vtces[vname1].nbrs.find(vname2) != vtces[vname1].nbrs.end()) {
            return;
        }
        vtces[vname1].nbrs[vname2] = value;
        vtces[vname2].nbrs[vname1] = value;
    }

    void removeEdge(const string& vname1, const string& vname2) {
        if (vtces.find(vname1) == vtces.end() || vtces.find(vname2) == vtces.end() || 
            vtces[vname1].nbrs.find(vname2) == vtces[vname1].nbrs.end()) {
            return;
        }
        vtces[vname1].nbrs.erase(vname2);
        vtces[vname2].nbrs.erase(vname1);
    }

    void display_Map() {
        cout << "\t Delhi Metro Map" << endl;
        cout << "\t------------------" << endl;
        cout << "----------------------------------------------------\n" << endl;
        
        for (const auto& pair : vtces) {  // C++11: auto and range-based for
            string str = pair.first + " =>\n";
            for (const auto& nbr : pair.second.nbrs) {
                str += "\t" + nbr.first + "\t";
                if (nbr.first.length() < 16) str += "\t";
                if (nbr.first.length() < 8) str += "\t";
                str += to_string(nbr.second) + "\n";  // C++11: to_string
            }
            cout << str;
        }
        cout << "\t------------------" << endl;
        cout << "---------------------------------------------------\n" << endl;
    }

    void display_Stations() {
        cout << "\n***********************************************************************\n" << endl;
        int i = 1;
        for (const auto& pair : vtces) {
            cout << i++ << ". " << pair.first << endl;
        }
        cout << "\n***********************************************************************\n" << endl;
    }

    bool hasPath(const string& vname1, const string& vname2, unordered_map<string, bool>& processed) {
        if (containsEdge(vname1, vname2)) {
            return true;
        }
        processed[vname1] = true;
        Vertex& vtx = vtces[vname1];
        for (const auto& nbr : vtx.nbrs) {
            if (!processed[nbr.first]) {
                if (hasPath(nbr.first, vname2, processed)) {
                    return true;
                }
            }
        }
        return false;
    }

    int dijkstra(const string& src, const string& des, bool nan) {
        int val = 0;
        unordered_map<string, DijkstraPair> map;
        vector<DijkstraPair> heap;

        // C++11: range-based for with auto
        for (const auto& pair : vtces) {
            DijkstraPair np;
            np.vname = pair.first;
            np.cost = INT_MAX;
            if (pair.first == src) {
                np.cost = 0;
                np.psf = pair.first;
            }
            heap.push_back(np);
            map[pair.first] = np;
        }

        // STL heap operations (C++11 compatible)
        make_heap(heap.begin(), heap.end());  // Creates min-heap with our < operator

        while (!heap.empty()) {
            pop_heap(heap.begin(), heap.end());  // Moves min to back
            DijkstraPair rp = heap.back();
            heap.pop_back();

            if (rp.vname == des) {
                val = rp.cost;
                break;
            }
            map.erase(rp.vname);

            Vertex& v = vtces[rp.vname];
            for (const auto& nbr : v.nbrs) {
                if (map.find(nbr.first) != map.end()) {
                    int oc = map[nbr.first].cost;
                    int nc = nan ? rp.cost + 120 + 40 * nbr.second : rp.cost + nbr.second;
                    if (nc < oc) {
                        DijkstraPair& gp = map[nbr.first];
                        gp.psf = rp.psf + nbr.first;
                        gp.cost = nc;

                        // Update heap by modifying element and re-heapifying
                        for (auto& item : heap) {  // C++11: range-based for with reference
                            if (item.vname == nbr.first) {
                                item.psf = gp.psf;
                                item.cost = nc;
                                break;
                            }
                        }
                        make_heap(heap.begin(), heap.end());
                    }
                }
            }
        }
        return val;
    }

    string getMinimumDistance(const string& src, const string& dst) {
        int min = INT_MAX;
        string ans;
        unordered_map<string, bool> processed;
        vector<Pair> stack;

        Pair sp;
        sp.vname = src;
        sp.psf = src + "  ";
        sp.min_dis = 0;
        sp.min_time = 0;
        stack.push_back(sp);

        while (!stack.empty()) {
            Pair rp = stack.back();
            stack.pop_back();

            if (processed[rp.vname]) continue;
            processed[rp.vname] = true;

            if (rp.vname == dst) {
                if (rp.min_dis < min) {
                    ans = rp.psf;
                    min = rp.min_dis;
                }
                continue;
            }

            Vertex& rpvtx = vtces[rp.vname];
            for (const auto& nbr : rpvtx.nbrs) {
                if (!processed[nbr.first]) {
                    Pair np;
                    np.vname = nbr.first;
                    np.psf = rp.psf + nbr.first + "  ";
                    np.min_dis = rp.min_dis + nbr.second;
                    stack.push_back(np);
                }
            }
        }
        return ans + to_string(min);  // C++11: to_string
    }

    string getMinimumTime(const string& src, const string& dst) {
        int min = INT_MAX;
        string ans;
        unordered_map<string, bool> processed;
        vector<Pair> stack;

        Pair sp;
        sp.vname = src;
        sp.psf = src + "  ";
        sp.min_dis = 0;
        sp.min_time = 0;
        stack.push_back(sp);

        while (!stack.empty()) {
            Pair rp = stack.back();
            stack.pop_back();

            if (processed[rp.vname]) continue;
            processed[rp.vname] = true;

            if (rp.vname == dst) {
                if (rp.min_time < min) {
                    ans = rp.psf;
                    min = rp.min_time;
                }
                continue;
            }

            Vertex& rpvtx = vtces[rp.vname];
            for (const auto& nbr : rpvtx.nbrs) {
                if (!processed[nbr.first]) {
                    Pair np;
                    np.vname = nbr.first;
                    np.psf = rp.psf + nbr.first + "  ";
                    np.min_time = rp.min_time + 120 + 40 * nbr.second;
                    stack.push_back(np);
                }
            }
        }
        double minutes = ceil(static_cast<double>(min) / 60);
        return ans + to_string(minutes);  // C++11: to_string
    }

    vector<string> getInterchanges(const string& str) {
        vector<string> arr;
        vector<string> res;
        stringstream ss(str);
        string token;
        while (getline(ss, token, ' ')) {
            if (!token.empty()) res.push_back(token);
        }
        arr.push_back(res[0]);
        int count = 0;
        for (size_t i = 1; i < res.size() - 1; i++) {
            size_t index = res[i].find('~');
            string s = res[i].substr(index + 1);
            if (s.length() == 2) {
                string prev = res[i-1].substr(res[i-1].find('~') + 1);
                string next = res[i+1].substr(res[i+1].find('~') + 1);
                if (prev == next) {
                    arr.push_back(res[i]);
                } else {
                    arr.push_back(res[i] + " ==> " + res[i+1]);
                    i++;
                    count++;
                }
            } else {
                arr.push_back(res[i]);
            }
        }
        arr.push_back(to_string(count));  // C++11: to_string
        arr.push_back(res.back());
        return arr;
    }

    static void createMetroMap(Graph_M& g) {
        // Same as before, adding vertices and edges
        g.addVertex("Noida Sector 62~B");
        g.addVertex("Botanical Garden~B");
        g.addVertex("Yamuna Bank~B");
        g.addVertex("Rajiv Chowk~BY");
        g.addVertex("Vaishali~B");
        g.addVertex("Moti Nagar~B");
        g.addVertex("Janak Puri West~BO");
        g.addVertex("Dwarka Sector 21~B");
        g.addVertex("Huda City Center~Y");
        g.addVertex("Saket~Y");
        g.addVertex("Vishwavidyalaya~Y");
        g.addVertex("Chandni Chowk~Y");
        g.addVertex("New Delhi~YO");
        g.addVertex("AIIMS~Y");
        g.addVertex("Shivaji Stadium~O");
        g.addVertex("DDS Campus~O");
        g.addVertex("IGI Airport~O");
        g.addVertex("Rajouri Garden~BP");
        g.addVertex("Netaji Subhash Place~PR");
        g.addVertex("Punjabi Bagh West~P");

        g.addEdge("Noida Sector 62~B", "Botanical Garden~B", 8);
        g.addEdge("Botanical Garden~B", "Yamuna Bank~B", 10);
        g.addEdge("Yamuna Bank~B", "Vaishali~B", 8);
        g.addEdge("Yamuna Bank~B", "Rajiv Chowk~BY", 6);
        g.addEdge("Rajiv Chowk~BY", "Moti Nagar~B", 9);
        g.addEdge("Moti Nagar~B", "Janak Puri West~BO", 7);
        g.addEdge("Janak Puri West~BO", "Dwarka Sector 21~B", 6);
        g.addEdge("Huda City Center~Y", "Saket~Y", 15);
        g.addEdge("Saket~Y", "AIIMS~Y", 6);
        g.addEdge("AIIMS~Y", "Rajiv Chowk~BY", 7);
        g.addEdge("Rajiv Chowk~BY", "New Delhi~YO", 1);
        g.addEdge("New Delhi~YO", "Chandni Chowk~Y", 2);
        g.addEdge("Chandni Chowk~Y", "Vishwavidyalaya~Y", 5);
        g.addEdge("New Delhi~YO", "Shivaji Stadium~O", 2);
        g.addEdge("Shivaji Stadium~O", "DDS Campus~O", 7);
        g.addEdge("DDS Campus~O", "IGI Airport~O", 8);
        g.addEdge("Moti Nagar~B", "Rajouri Garden~BP", 2);
        g.addEdge("Punjabi Bagh West~P", "Rajouri Garden~BP", 2);
        g.addEdge("Punjabi Bagh West~P", "Netaji Subhash Place~PR", 3);
    }



//	static void createMetroMap(Graph_M& g) {
//	    // Corrected existing stations with consistent naming and accurate line codes
//	    g.addVertex("Noida Sector 62~B");
//	    g.addVertex("Botanical Garden~B");
//	    g.addVertex("Yamuna Bank~B");
//	    g.addVertex("Rajiv Chowk~BY");
//	    g.addVertex("Vaishali~B");
//	    g.addVertex("Moti Nagar~B");
//	    g.addVertex("Janakpuri West~B"); // Consistent naming
//	    g.addVertex("Dwarka Sector 21~B");
//	    g.addVertex("Huda City Centre~YV"); // Corrected spelling and line code
//	    g.addVertex("Saket~Y");
//	    g.addVertex("Vishwavidyalaya~Y");
//	    g.addVertex("Chandni Chowk~Y");
//	    g.addVertex("New Delhi~YBO");
//	    g.addVertex("AIIMS~Y");
//	    g.addVertex("Shivaji Stadium~O");
//	    g.addVertex("DDS Campus~O");
//	    g.addVertex("IGI Airport~O");
//	    g.addVertex("Rajouri Garden~BP");
//	    g.addVertex("Netaji Subhash Place~PR");
//	    g.addVertex("Punjabi Bagh West~PG");
//	
//	    // Red Line (~R)
//	    vector<string> redLine = {
//	        "Shaheed Sthal~R", "Hindon River~R", "Arthala~R", "Mohan Nagar~R", "Shyam Park~R",
//	        "Major Mohit Sharma Rajendra Nagar~R", "Raj Bagh~R", "Shaheed Nagar~R", "Dilshad Garden~R",
//	        "Jhilmil~R", "Mansarovar Park~R", "Shahdara~R", "Welcome~R", "Seelampur~R", "Shastri Park~R",
//	        "Kashmere Gate~RYV", "Tis Hazari~R", "Pul Bangash~R", "Pratap Nagar~R", "Shastri Nagar~R",
//	        "Inderlok~RG", "Kanhaiya Nagar~R", "Keshav Puram~R", "Netaji Subhash Place~PR",
//	        "Kohat Enclave~R", "Pitampura~R", "Rohini East~R", "Rohini West~R", "Rithala~R"
//	    };
//	    for (size_t i = 0; i < redLine.size() - 1; ++i) {
//	        g.addVertex(redLine[i]);
//	        g.addEdge(redLine[i], redLine[i + 1], 2);
//	    }
//	
//	    // Yellow Line (~Y)
//	    vector<string> yellowLine = {
//	        "Samaypur Badli~Y", "Rohini Sector 18,19~Y", "Haiderpur Badli Mor~Y", "Jahangirpuri~Y",
//	        "Adarsh Nagar~Y", "Azadpur~YP", "Model Town~Y", "GTB Nagar~Y", "Vishwavidyalaya~Y",
//	        "Vidhan Sabha~Y", "Civil Lines~Y", "Kashmere Gate~RYV", "Chandni Chowk~Y", "Chawri Bazar~Y",
//	        "New Delhi~YBO", "Rajiv Chowk~BY", "Patel Chowk~Y", "Central Secretariat~YV", "Udyog Bhawan~Y",
//	        "Lok Kalyan Marg~Y", "Jor Bagh~Y", "INA~Y", "AIIMS~Y", "Green Park~Y", "Hauz Khas~Y",
//	        "Malviya Nagar~Y", "Saket~Y", "Qutab Minar~Y", "Chhatarpur~Y", "Sultanpur~Y", "Ghitorni~Y",
//	        "Arjan Garh~Y", "Guru Dronacharya~Y", "Sikandarpur~Y", "MG Road~Y", "IFFCO Chowk~Y",
//	        "Huda City Centre~YV"
//	    };
//	    for (size_t i = 0; i < yellowLine.size() - 1; ++i) {
//	        g.addVertex(yellowLine[i]);
//	        g.addEdge(yellowLine[i], yellowLine[i + 1], 2);
//	    }
//	
//	    // Blue Line (~B) - Main Line
//	    vector<string> blueLineMain = {
//	        "Dwarka Sector 21~B", "Dwarka Sector 8~B", "Dwarka Sector 9~B", "Dwarka Sector 10~B",
//	        "Dwarka Sector 11~B", "Dwarka Sector 12~B", "Dwarka Sector 13~B", "Dwarka Sector 14~B",
//	        "Dwarka~B", "Dwarka Mor~B", "Nawada~B", "Uttam Nagar West~B", "Uttam Nagar East~B",
//	        "Janakpuri West~B", "Janakpuri East~B", "Tilak Nagar~B", "Subhash Nagar~B", "Tagore Garden~B",
//	        "Rajouri Garden~BP", "Ramesh Nagar~B", "Moti Nagar~B", "Kirti Nagar~BG", "Shadipur~B",
//	        "Patel Nagar~B", "Rajendra Place~B", "Karol Bagh~B", "Jhandewalan~B", "RK Ashram Marg~B",
//	        "Rajiv Chowk~BY", "Barakhamba Road~B", "Mandi House~BV", "Pragati Maidan~B", "Indraprastha~B",
//	        "Yamuna Bank~B", "Laxmi Nagar~B", "Nirman Vihar~B", "Preet Vihar~B", "Karkarduma~B",
//	        "Anand Vihar~B", "Kaushambi~B", "Vaishali~B"
//	    };
//	    for (size_t i = 0; i < blueLineMain.size() - 1; ++i) {
//	        g.addVertex(blueLineMain[i]);
//	        g.addEdge(blueLineMain[i], blueLineMain[i + 1], 2);
//	    }
//	
//	    // Green Line (~G)
//	    vector<string> greenLine = {
//	        "Inderlok~RG", "Ashok Park Main~G", "Punjabi Bagh West~PG", "Shivaji Park~G", "Madipur~G",
//	        "Paschim Vihar East~G", "Paschim Vihar West~G", "Peera Garhi~G", "Udyog Nagar~G",
//	        "Surajmal Stadium~G", "Nangloi~G", "Nangloi Railway Station~G", "Rajdhani Park~G",
//	        "Mundka~G", "Mundka Industrial Area~G", "Ghevra~G", "Tikri Kalan~G", "Tikri Border~G",
//	        "Pandit Shree Ram Sharma~G", "Bahadurgarh City~G", "Brigadier Hoshiar Singh~G"
//	    };
//	    for (size_t i = 0; i < greenLine.size() - 1; ++i) {
//	        g.addVertex(greenLine[i]);
//	        g.addEdge(greenLine[i], greenLine[i + 1], 2);
//	    }
//	
//	    // Violet Line (~V)
//	    vector<string> violetLine = {
//	        "Kashmere Gate~RYV", "Lal Qila~V", "Jama Masjid~V", "Delhi Gate~V", "ITO~V", "Mandi House~BV",
//	        "Janpath~V", "Central Secretariat~YV", "Khan Market~V", "Jawaharlal Nehru Stadium~V", "Jangpura~V",
//	        "Lajpat Nagar~V", "Moolchand~V", "Kailash Colony~V", "Nehru Place~V", "Kalkaji Mandir~V",
//	        "Govind Puri~V", "Okhla~V", "Jasola~V", "Sarita Vihar~V", "Mohan Estate~V", "Tughlakabad~V",
//	        "Badarpur Border~V", "Sarai~V", "NHPC Chowk~V", "Mewala Maharajpur~V", "Sector 28~V",
//	        "Badkal Mor~V", "Old Faridabad~V", "Neelam Chowk Ajronda~V", "Bata Chowk~V", "Escorts Mujesar~V",
//	        "Raja Nahar Singh~V"
//	    };
//	    for (size_t i = 0; i < violetLine.size() - 1; ++i) {
//	        g.addVertex(violetLine[i]);
//	        g.addEdge(violetLine[i], violetLine[i + 1], 2);
//	    }
//	
//	    // Pink Line (~P)
//	    vector<string> pinkLine = {
//	        "Majlis Park~P", "Azadpur~YP", "Shalimar Bagh~P", "Netaji Subhash Place~PR", "Shakurpur~P",
//	        "Punjabi Bagh West~PG", "ESI Hospital~P", "Rajouri Garden~BP", "Mayapuri~P", "Naraina Vihar~P",
//	        "Delhi Cantt~P"
//	    };
//	    for (size_t i = 0; i < pinkLine.size() - 1; ++i) {
//	        g.addVertex(pinkLine[i]);
//	        g.addEdge(pinkLine[i], pinkLine[i + 1], 2);
//	    }
//	
//	    // Corrected original edges with consistent naming
//	    g.addEdge("Noida Sector 62~B", "Botanical Garden~B", 8);
//	    g.addEdge("Botanical Garden~B", "Yamuna Bank~B", 10);
//	    g.addEdge("Yamuna Bank~B", "Vaishali~B", 8);
//	    g.addEdge("Yamuna Bank~B", "Rajiv Chowk~BY", 6);
//	    g.addEdge("Rajiv Chowk~BY", "Moti Nagar~B", 9);
//	    g.addEdge("Moti Nagar~B", "Janakpuri West~B", 7);
//	    g.addEdge("Janakpuri West~B", "Dwarka Sector 21~B", 6);
//	    g.addEdge("Huda City Centre~YV", "Saket~Y", 15);
//	    g.addEdge("Saket~Y", "AIIMS~Y", 6);
//	    g.addEdge("AIIMS~Y", "Rajiv Chowk~BY", 7);
//	    g.addEdge("Rajiv Chowk~BY", "New Delhi~YBO", 1);
//	    g.addEdge("New Delhi~YBO", "Chandni Chowk~Y", 2);
//	    g.addEdge("Chandni Chowk~Y", "Vishwavidyalaya~Y", 5);
//	    g.addEdge("New Delhi~YBO", "Shivaji Stadium~O", 2);
//	    g.addEdge("Shivaji Stadium~O", "DDS Campus~O", 7);
//	    g.addEdge("DDS Campus~O", "IGI Airport~O", 8);
//	    g.addEdge("Moti Nagar~B", "Rajouri Garden~BP", 2);
//	    g.addEdge("Punjabi Bagh West~PG", "Rajouri Garden~BP", 2);
//	    g.addEdge("Punjabi Bagh West~PG", "Netaji Subhash Place~PR", 3);
//	
//	    // Add explicit interchange connections (0 km distance for same station)
//	    g.addEdge("Kashmere Gate~RYV", "Kashmere Gate~RYV", 0); // Red-Yellow-Violet
//	    g.addEdge("Rajiv Chowk~BY", "Rajiv Chowk~BY", 0); // Blue-Yellow
//	    g.addEdge("New Delhi~YBO", "New Delhi~YBO", 0); // Yellow-Blue-Orange
//	    g.addEdge("Inderlok~RG", "Inderlok~RG", 0); // Red-Green
//	    g.addEdge("Netaji Subhash Place~PR", "Netaji Subhash Place~PR", 0); // Pink-Red
//	    g.addEdge("Rajouri Garden~BP", "Rajouri Garden~BP", 0); // Blue-Pink
//	    g.addEdge("Punjabi Bagh West~PG", "Punjabi Bagh West~PG", 0); // Pink-Green
//	    g.addEdge("Mandi House~BV", "Mandi House~BV", 0); // Blue-Violet
//	    g.addEdge("Central Secretariat~YV", "Central Secretariat~YV", 0); // Yellow-Violet
//	    g.addEdge("Azadpur~YP", "Azadpur~YP", 0); // Yellow-Pink
//	}

    vector<string> printCodelist() {
        cout << "List of stations along with their codes:\n" << endl;
        vector<string> keys;
        for (const auto& pair : vtces) {
            keys.push_back(pair.first);
        }
        vector<string> codes(keys.size());
        int i = 1, m = 1;
        for (const string& key : keys) {
            stringstream ss(key);
            string temp, code;
            while (ss >> temp) {
                char c = temp[0];
                int j = 0;
                while (isdigit(c)) {
                    code += c;
                    c = temp[++j];
                }
                if (!isupper(c)) code += c;
            }
            if (code.length() < 2) {
                code += toupper(key[key.find('~') + 2]);
            }
            codes[i-1] = code;
            cout << i << ". " << key << "\t";
            if (key.length() < (22-m)) cout << "\t";
            if (key.length() < (14-m)) cout << "\t";
            if (key.length() < (6-m)) cout << "\t";
            cout << codes[i-1] << endl;
            i++;
            if (i == pow(10, m)) m++;
        }
        return codes;
    }

    // Add a public method to get vertices for use in main()
    vector<string> getVertices() const {
        vector<string> keys;
        for (const auto& pair : vtces) {
            keys.push_back(pair.first);
        }
        return keys;
    }
};

int main() {
    Graph_M g;
    Graph_M::createMetroMap(g);

    cout << "\n\t\t\t****WELCOME TO THE METRO APP*****" << endl;
    
    while (true) {
        cout << "\t\t\t\t~~LIST OF ACTIONS~~\n\n";
        cout << "1. LIST ALL THE STATIONS IN THE MAP\n";
        cout << "2. SHOW THE METRO MAP\n";
        cout << "3. GET SHORTEST DISTANCE FROM A 'SOURCE' STATION TO 'DESTINATION' STATION\n";
        cout << "4. GET SHORTEST TIME TO REACH FROM A 'SOURCE' STATION TO 'DESTINATION' STATION\n";
        cout << "5. GET SHORTEST PATH (DISTANCE WISE) TO REACH FROM A 'SOURCE' STATION TO 'DESTINATION' STATION\n";
        cout << "6. GET SHORTEST PATH (TIME WISE) TO REACH FROM A 'SOURCE' STATION TO 'DESTINATION' STATION\n";
        cout << "7. EXIT THE MENU\n";
        cout << "\nENTER YOUR CHOICE FROM THE ABOVE LIST (1 to 7) : ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        cout << "\n***********************************************************\n";

        if (choice == 7) break;

        switch (choice) {
            case 1:
                g.display_Stations();
                break;
            case 2:
                g.display_Map();
                break;
            case 3: {
                vector<string> keys = g.getVertices();  // Use public method instead of direct access
                vector<string> codes = g.printCodelist();
                cout << "\n1. TO ENTER SERIAL NO. OF STATIONS\n2. TO ENTER CODE OF STATIONS\n3. TO ENTER NAME OF STATIONS\n";
                cout << "ENTER YOUR CHOICE: ";
                int ch;
                cin >> ch;
                cin.ignore();
                
                string st1, st2;
                cout << "ENTER THE SOURCE AND DESTINATION STATIONS\n";
                if (ch == 1) {
                    int idx1, idx2;
                    cin >> idx1 >> idx2;
                    cin.ignore();
                    st1 = keys[idx1-1];
                    st2 = keys[idx2-1];
                } else if (ch == 2) {
                    string a, b;
                    getline(cin, a);
                    getline(cin, b);
                    transform(a.begin(), a.end(), a.begin(), ::toupper);  // C++11: transform with lambda-like function
                    transform(b.begin(), b.end(), b.begin(), ::toupper);
                    int j = 0;
                    for (; j < codes.size(); j++) if (a == codes[j]) break;
                    st1 = keys[j];
                    j = 0;
                    for (; j < codes.size(); j++) if (b == codes[j]) break;
                    st2 = keys[j];
                } else if (ch == 3) {
                    getline(cin, st1);
                    getline(cin, st2);
                } else {
                    cout << "Invalid choice" << endl;
                    return 0;
                }

                unordered_map<string, bool> processed;
                if (!g.containsVertex(st1) || !g.containsVertex(st2) || !g.hasPath(st1, st2, processed)) {
                    cout << "THE INPUTS ARE INVALID" << endl;
                } else {
                    cout << "SHORTEST DISTANCE FROM " << st1 << " TO " << st2 << " IS " 
                         << g.dijkstra(st1, st2, false) << "KM\n" << endl;
                }
                break;
            }
            case 4: {
                cout << "ENTER THE SOURCE STATION: ";
                string sat1;
                getline(cin, sat1);
                cout << "ENTER THE DESTINATION STATION: ";
                string sat2;
                getline(cin, sat2);

                unordered_map<string, bool> processed;
                if (!g.containsVertex(sat1) || !g.containsVertex(sat2) || !g.hasPath(sat1, sat2, processed)) {
                    cout << "THE INPUTS ARE INVALID" << endl;
                } else {
                    cout << "SHORTEST TIME FROM (" << sat1 << ") TO (" << sat2 << ") IS " 
                         << g.dijkstra(sat1, sat2, true)/60 << " MINUTES\n\n";
                }
                break;
            }
            case 5: {
                cout << "ENTER THE SOURCE AND DESTINATION STATIONS\n";
                string s1, s2;
                getline(cin, s1);
                getline(cin, s2);

                unordered_map<string, bool> processed;
                if (!g.containsVertex(s1) || !g.containsVertex(s2) || !g.hasPath(s1, s2, processed)) {
                    cout << "THE INPUTS ARE INVALID" << endl;
                } else {
                    vector<string> str = g.getInterchanges(g.getMinimumDistance(s1, s2));
                    int len = str.size();
                    cout << "SOURCE STATION : " << s1 << endl;
                    cout << "DESTINATION STATION : " << s2 << endl;
                    cout << "DISTANCE : " << str[len-1] << endl;
                    cout << "NUMBER OF INTERCHANGES : " << str[len-2] << endl;
                    cout << "~~~~~~~~~~~~~\n";
                    cout << "START  ==>  " << str[0] << endl;
                    for (int i = 1; i < len-3; i++) {
                        cout << str[i] << endl;
                    }
                    cout << str[len-3] << "   ==>    END\n";
                    cout << "~~~~~~~~~~~~~\n";
                }
                break;
            }
            case 6: {
                cout << "ENTER THE SOURCE STATION: ";
                string ss1, ss2;
                getline(cin, ss1);
                cout << "ENTER THE DESTINATION STATION: ";
                getline(cin, ss2);

                unordered_map<string, bool> processed;
                if (!g.containsVertex(ss1) || !g.containsVertex(ss2) || !g.hasPath(ss1, ss2, processed)) {
                    cout << "THE INPUTS ARE INVALID" << endl;
                } else {
                    vector<string> str = g.getInterchanges(g.getMinimumTime(ss1, ss2));
                    int len = str.size();
                    cout << "SOURCE STATION : " << ss1 << endl;
                    cout << "DESTINATION STATION : " << ss2 << endl;
                    cout << "TIME : " << str[len-1] << " MINUTES" << endl;
                    cout << "NUMBER OF INTERCHANGES : " << str[len-2] << endl;
                    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
                    cout << "START  ==>  " << str[0] << " ==>  ";
                    for (int i = 1; i < len-3; i++) {
                        cout << str[i] << endl;
                    }
                    cout << str[len-3] << "   ==>    END\n";
                    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
                }
                break;
            }
            default:
                cout << "Please enter a valid option! " << endl;
                cout << "The options you can choose are from 1 to 7. " << endl;
        }
    }
    return 0;
}
