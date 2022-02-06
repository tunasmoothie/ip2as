#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

string dec_to_bin(int *dec_list){
	string bin;
	for(int i = 3; i >= 0; i--){
		
		for(int j = 0; j < 8; j++){
			bin = ( (dec_list[i] % 2 == 0) ? "0" : "1" ) + bin;
			dec_list[i] /= 2;
		}
	}
	return bin;
}	

string extract_prefix_from_ip(string ip, int cidr){
	replace(ip.begin(), ip.end(), '.', ' ');
	istringstream iss(ip);
	int chunks[4];
	iss >> chunks[0] >> chunks[1] >> chunks[2] >> chunks[3];
	
	string prefix = dec_to_bin(chunks);
	prefix = prefix.substr(0, cidr);
	
	return prefix;
}

class Node{
	private:
	    // data
		string ip;
		string prefix;
		int as;
		int cidr;
		bool empty;
		
		// node linking
		Node *node1 = nullptr;
		Node *node0 = nullptr;
		
	public:
		Node(){
			ip = "";
			prefix = "";
			as = -1;
			cidr = -1;
			empty = true;
		}
		
		Node(string line){
			istringstream iss(line);
			iss >> ip;
			iss >> cidr;
			iss >> as;
			empty = false;
			
			prefix = extract_prefix_from_ip(ip, cidr);
		}
		
		string get_ip(){return ip;}
		string get_prefix(){return prefix;}
		int get_as(){return as;}
		int get_cidr(){return cidr;}
		
		void copy_data(Node *n){
			ip = n->get_ip();
			prefix = n->get_prefix();
			as = n->get_as();
			cidr = n->get_cidr();
		}
		
		void set_child(int i, Node *n){
			(i == 0) ? node0 = n : node1 = n;
		}
		
		void set_empty(bool b){
			empty = b;
		}
		
		Node *get_child(int i){
			if(!has_child(i)) 
				return nullptr;
			else
				return (i == 0) ? node0 : node1;
		}
		
		bool has_child(int i){
			return (i == 0) ? node0 != nullptr : node1 != nullptr;
		}
		
		bool is_empty(){
			return empty;
		}		

};

class Trie{
	private:
		ifstream db;
		Node *root;
	public:
		Trie(string filename){
			db.open(filename);
		}
		
		// builds Trie with Nodes from a given database file
		void build(){
			root = new Node();
			
			string line;
			
			while(getline(db, line)){
				Node *curr = root;
				Node n(line);
				string bits = n.get_prefix();
				
				// for each bit in the prefix, check if node has child and create if it doesn't
				for(int i = 0; i < bits.length(); i++){
					int w = (bits[i] == '0') ? 0 : 1;
					
					if(curr->has_child(w)){
						curr = curr->get_child(w);
					}
					else{
						curr->set_child(w, new Node());
						curr = curr->get_child(w);
					}
				}
				
				// copies the new node's data into the trie
				curr->copy_data(&n);
				curr->set_empty(false);
			}
		}
		
		// matches given ip with longest prefix in trie through crawling, returns nullptr if none match
		Node *match(string ip){
			Node *curr = root;
			Node *found = nullptr;
			string ip_bin = extract_prefix_from_ip(ip, 32);
			
			for(int i = 0; i <= 32; i++){
				int w = (ip_bin[i] == '0') ? 0 : 1;
				
				if(curr->has_child(w)){
					curr = curr->get_child(w);
					
					// saves the longest matched prefix found at this point
					if(!curr->is_empty()){
						found = curr;
					}
				}
				else{
					return found;
				}
				
			}

			return nullptr;
		}
		
		// calls match() looping through a file with a list of IPs
		void match_from_list(string filename){
			ifstream fs(filename);
			
			string line;
			while(getline(fs, line)){
				Node *n = match(line);
				if(n == nullptr)
					cout << "No matching prefix found for " << line << endl;
				else
					cout << n->get_ip() << "/" << n->get_cidr() << "   " << n->get_as() << "   " << line << endl;
			}
			
		}
		
		Node *get_root(){
			return root;
		}
		
};

int main(int argc, char **argv){
	
	// cmd line processing
	if(argc != 3){
		cout << "\nError: unrecognized or incomplete command line.\n\n" <<
				"USAGE:\n" <<
				"    ip2as [DB file] [IP file]\n\n";
		exit(1);
	}
	
	string database = argv[1];
	string iplist = argv[2];
	
	Trie *tr = new Trie(database);
	tr->build();
	tr->match_from_list(iplist);

	return 0;
}

