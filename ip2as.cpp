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
			root = new Node();
		}
		
		// builds Trie with Nodes from a given database file
		void build(){
			string line;
			
			while(getline(db, line)){
				Node *curr = root;
				Node n(line);
				string bits = n.get_prefix();
				
				// for each bit in the prefix, check if node has child and create if it doesn't
				for(int i = 0; i < bits.length(); i++){
					if(bits[i] == '0'){
						if(!curr->has_child(0)){
							curr->set_child(0, new Node());
						}
						curr = curr->get_child(0);
					}
					else if(bits[i] == '1'){
						if(!curr->has_child(1)){
							curr->set_child(1, new Node());
						}
						curr = curr->get_child(1);
					}
				}
				
				// copies the new node's data into the trie
				if(curr->is_empty()){
					curr->copy_data(&n);
					curr->set_empty(false);
				}
				
				//cout << curr->get_ip() << "  " << curr->get_cidr() << "   " << curr->get_prefix() << endl;
				
				
				//cout << curr->get_ip() << "  " << curr->get_prefix() << endl;
			}
		}
		
		// matches given ip with longest prefix in trie through crawling
		Node *match(string ip){
			Node *curr = root;
			for(int i = 0; i < ip.length(); i++){
				cout << "Current Node: " << curr->get_ip() << "  " << curr->get_cidr() << endl;
				
				if(ip[i] == '0'){
					if(curr->has_child(0))
						curr = curr->get_child(0);
					else
						return curr;
				}
				else if(ip[i] == '1'){
					if(curr->has_child(1))
						curr = curr->get_child(1);
					else
						return curr;
				}
				
				//curr = curr->get_child(1);
				//(ip[i] == 0) ? curr = curr->get_child(0) : curr = curr->get_child(1);
			}
			
			cout << "No matching prefix found for " << ip << endl;
			return nullptr;
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
	
	Node *m = tr->match("192.0.0.12");
	//cout << m->get_ip() << "  " << m->get_cidr() << endl; 
	
	//cout << tr->get_root()->get_ip() << endl;
	//cout << tr->get_root()->get_child(1)->get_child(1)->get_ip() << endl;
	//cout << tr->get_root()->get_child(1)->get_child(0)->get_ip() << endl;
	//cout << tr->get_root()->get_child(1)->get_child(1)->get_prefix() << endl;
	//cout << tr->get_root()->get_child(1)->has_child(0) << endl;
	
	//Node n("192.168.34.3 16 69");
	
	return 0;
}

