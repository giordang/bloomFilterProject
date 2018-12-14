//Greg Giordano
//Bloom filter project


#include <iostream>
#include <vector>
#include <math.h>
using namespace std;


//constants
const int seed_m = 4; //seed value for murmur hash function
const float prob = 0.05; //desired probability for false positive in bloom filter
const int num_hash = 4; //number of hash functions
const int starting_n = 90; //number of elements loaded into bloom filter at start


//forward declarations
unsigned int sdbm(string key);
unsigned int djb2(string key);
unsigned int MurmurHash2 (const string* key, int len, unsigned int seed);
unsigned int greg_hash (string key);
unsigned int mod_bucket(unsigned int hashcode, unsigned int cap);
int vector_size(int n);
void lowercase(string& input);


//init_bit_vector returns a boolean vector of size n with all elements set to false
vector<bool> init_bit_vector(int n){
	vector<bool> ret;
	for (int i = 0; i < n; i++) { 
		ret.push_back(false); //add n false elements to vector
	}
	return ret;
}


//load_bit_vector returns a boolean vector. it takes a vector<string>, determines the
//appropriate size of the bloom filter based on the desired probability of false positives,
//hashes each element with 4 hash functions, and uses each hash to index the element in the new 
//boolean vector to true.
vector<bool> load_bit_vector(vector<string> movies){
  int boolVectSize = vector_size(movies.size()); //determine size of bloom filter
	vector<bool> boolVect = init_bit_vector(boolVectSize); //make bloom filter 
  string* key; //pointer to string for murmur hash

  for (int i = 0; i < movies.size(); i++) { //for each element in movies string
    //djb2 hash function
    int hash_d = djb2(movies[i]);
    int bucket_d = mod_bucket(hash_d, boolVect.size());
    boolVect[bucket_d] = true;

    //murmur hash function
    key = &movies[i];
    int hash_m = MurmurHash2(key, movies[i].length(), seed_m); 
    int bucket_m = mod_bucket(hash_m, boolVect.size());
    boolVect[bucket_m] = true;

    //sbdm hash function
    int hash_s = sdbm(movies[i]); 
    int bucket_s = mod_bucket(hash_s, boolVect.size());
    boolVect[bucket_s] = true;

    //greg hash function
    int hash_g = greg_hash(movies[i]); 
    int bucket_g = mod_bucket(hash_g, boolVect.size());
    boolVect[bucket_g] = true;
	}
	return boolVect;
}


//check_vector returns true if the string check is most likely present in the bloom
//filter or false if the string check is definitely not in the bloom filter
bool check_vector(vector<bool> bloom, string check){
  lowercase(check); //convert string to all lowercase

  //djb2 hash
  int hash_d = djb2(check);
  int bucket_d = mod_bucket(hash_d, bloom.size());

  //murmur hash
  string* key = &check;
  int hash_m = MurmurHash2(key, check.length(), seed_m);
  int bucket_m = mod_bucket(hash_m, bloom.size());

  //sdbm hash
  int hash_s = sdbm(check); 
  int bucket_s = mod_bucket(hash_s, bloom.size());

  //greg hash function
  int hash_g = greg_hash(check); 
  int bucket_g = mod_bucket(hash_g, bloom.size());

  //return false if false in any of the buckets from any hash function, return true otherwise
  if (bloom[bucket_d] == false || bloom[bucket_m] == false || bloom[bucket_s] == false || bloom[bucket_g] == false){ 
    return false;
  } else{
    return true;
  }
}


//add_string takes a bloom filter by reference and adds a string to it. it returns true if the new string
//creates at least one new index in the bloom filter, false otherwise.
bool add_string(vector<bool>& bloom, string add){
  lowercase(add); //convert string to all lowercase
  if (check_vector(bloom, add) == true){ //don't add if all indices already set to true
    return false;
  } else {
    //djb2 hash
    int hash_d = djb2(add);
    int bucket_d = mod_bucket(hash_d, bloom.size());
    bloom[bucket_d] = true;

    //murmur hash
    string* key = &add;
    int hash_m = MurmurHash2(key, add.length(), seed_m);
    int bucket_m = mod_bucket(hash_m, bloom.size());
    bloom[bucket_m] = true;
    
    //sdbm hash
    int hash_s = sdbm(add); 
    int bucket_s = mod_bucket(hash_s, bloom.size());
    bloom[bucket_s] = true;

    //greg hash function
    int hash_g = greg_hash(add); 
    int bucket_g = mod_bucket(hash_g, bloom.size());
    bloom[bucket_g] = true;

    return true;
  }
}


//bloom_info takes a bloom filter and prints information about the current state of the
//bloom filter. The output includes the preset desired probability of false positives,
//the actual probability of false positives, the actual number of hash functions being
//used, the optimal number of hash functions to use, the starting number of items in the
//bloom filter, an estimate of the current number of items in the bloom filter, and also
//a full printout of bloom filter.
//reference: equations taken from bloom filter wikipedia 
//https://en.wikipedia.org/wiki/Bloom_filter 
void bloom_info(vector<bool>bloom){
  cout<< "********************************" <<endl;
  //desired probability of false positives
  cout<< "Desired probability of false positives: ";
  cout<< prob <<endl;

  //probability of false positives
  //p = (1 - [1 - 1/m]^kn)^k
  //m = size of bloom filter
  //k = number of hash functions
  //n = number of items in bloom filter
  cout<< "Actual probability of false positives: ";
  double bloomSize = bloom.size();
  double bloomCalc = 1.0 / bloomSize;
  bloomCalc = 1.0 - bloomCalc;
  double n = starting_n;
  double hashes = num_hash;
  double kn = n * hashes;
  bloomCalc = pow(bloomCalc, kn);
  bloomCalc = 1 - bloomCalc;
  bloomCalc = pow(bloomCalc, hashes);
  cout<< bloomCalc <<endl;

  //actual number of hash functions
  cout<< "Actual number of hash functions: ";
  cout<< num_hash <<endl;

  //optimal number of hash functions
  //k = (m/n)*ln2
  //m = size of bloom filter
  //n = number of items in bloom filter
  cout<< "Optimal number of hash functions: ";
  float bloomHash = (bloomSize / n) * log(2);
  cout<< bloomHash <<endl;
  
  //starting number of items in bloom filter
  cout<< "Number of items in bloom filter at start: ";
  cout<< starting_n <<endl;

  //estimate number of items in bloom filter
  //n estimate = -(m/k)*ln[1 - X/m]
  //X = number of trues in bloom filter
  cout<< "Current estimate of number of items in bloom filter: ";
  int count = 0;	
  for (int i = 0; i < bloom.size(); i++) {
		if(bloom[i] == true){
      count++;
    }
	}
  float temp = 1 - (count / bloomSize);
  float n_estimate =(-1 * bloomSize / hashes) * log(temp);
  cout<< n_estimate <<endl;
  
  //print entire bloom filter
  cout<< "Bloom filter:" <<endl;
	for (int i = 0; i < bloom.size(); i++) {
		cout << bloom[i];
	}
  cout<< "" <<endl;
  cout<< "********************************" <<endl;
}


//lowercase takes a string by reference and converts each character to lowercase, if possible
void lowercase(string& input){
  for(int i = 0; i < input.length(); i++){
    input[i] = tolower(input[i]);
  }
}


//vector size takes an integer n(number of elements to be inserted into bloom filter)
//and calculates the appropriate size of the bloom filter to achieve the desired 
//probability of false positives(prob)
//reference: equation taken from bloom filter wikipedia
//https://en.wikipedia.org/wiki/Bloom_filter
//m = -(n*ln p)/(ln 2)^2
//m = size of bloom filter
//n = number of items in bloom filter
//p = desired probability of false positives
int vector_size(int n){
  int bestSize = ceil(-1 * (n * log(prob)) / (pow(log(2), 2))); 
  return bestSize; 
}

//mod bucket finds the bucket for the given hash in the vector of the given
//size of the vector vectSize
//reference: hash homework assignment
unsigned int mod_bucket(unsigned int hash, unsigned int vectSize) {
  unsigned int ret = hash % vectSize;
  return ret;
}


//greg_hash, the mightiest of all hash functions
//takes string key and returns unsigned int hash
//reference: made from discussion of multiplicative hash functions: 
//https://www.codeproject.com/Articles/32829/Hash-Functions-An-Empirical-Comparison
unsigned int greg_hash (string key) {
  unsigned int hash = 4444; //initial value
  for (size_t i = 0; i < key.length(); i++) {
    char c = key[i]; 
    hash = 44 * hash + c; //multiplier
  }
  return hash;
}


//sbdm takes string key and returns unsigned int hash
//reference: http://www.cse.yorku.ca/~oz/hash.html#sdbm
unsigned int sdbm(string key){
  unsigned int hash = 0;
  for (size_t i = 0; i < key.length(); i++) {
    char c = key[i]; 
    hash = c + (hash << 6) + (hash << 16) - hash; 
  }
  return hash;
}


//djb2 takes a string key and returns unsigned int hash
//reference: hash table homework assignment
unsigned int djb2(string key) {
  unsigned int hash = 5381;
  for (size_t i = 0; i < key.length(); i++) {
    char c = key[i]; 
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}


//MurmurHash2 takes a pointer to a string, length of string, and a seed value to
//return an unsigned int hash
//reference: taken from Code Project link from hash hw
//https://www.codeproject.com/Articles/32829/Hash-Functions-An-Empirical-Comparison
unsigned int MurmurHash2 (const string* key, int len, unsigned int seed){
	const unsigned int m = 0x5bd1e995;
	const int r = 24;
	unsigned int h = seed ^ len;
	const unsigned char * data = (const unsigned char *)key;
	while(len >= 4){
		unsigned int k = *(unsigned int *)data;
		k *= m; 
		k ^= k >> r; 
		k *= m; 
		h *= m; 
		h ^= k;
		data += 4;
		len -= 4;
	}

	switch(len) {
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
} 


//main function includes simple command line interface to output bloom filter info,
//add, and check items in bloom filter. On startup, it creates a bloom filter with all
//academy award best picture winners.
int main () {
  //create string vector of best pictures
  vector<string> bestPicsVect;
  string bestPicsArray[90] = {"wings", "the broadway melody", "all quiet on the western front", "cimarron", "grand hotel", "cavalcade", "it happened one night", "mutiny on the bounty", "the great ziegfeld", "the life of émile zola", "you can't take it with you", "gone with the wind", "rebecca", "how green was my valley", "mrs. miniver", "casablanca", "going my way", "the lost weekend", "the best years of our lives", "gentleman's agreement", "hamlet", "all the king's men", "all about eve", "an american in paris", "the greatest show on earth", "from here to eternity", "on the waterfront", "marty", "around the world in eighty days", "the bridge on the river kwai", "gigi", "ben-hur", "the apartment", "west side story", "lawrence of arabia", "tom jones", "my fair lady", "the sound of music", "a man for all seasons", "in the heat of the night", "oliver!", "midnight cowboy", "patton", "the french connection", "the godfather", "the sting", "the godfather, part ii", "one flew over the cuckoo's nest", "rocky", "annie hall", "the deer hunter", "kramer vs. kramer", "ordinary people", "chariots of fire", "gandhi", "terms of endearment", "amadeus", "out of africa", "platoon", "the last emperor", "rain man", "driving miss daisy", "dances with wolves", "the silence of the lambs", "unforgiven", "schindler's list", "forrest gump", "braveheart", "the english patient", "titanic", "shakespeare in love", "american beauty", "gladiator", "a beautiful mind", "chicago", "the lord of the rings: the return of the king", "million dollar baby", "crash", "the departed", "no country for old men", "slumdog millionaire", "the hurt locker", "the king's speech", "the artist", "argo", "12 years a slave", "birdman", "spotlight", "moonlight", "the shape of water"};
  for (int i = 0; i < 90; i++) {
		bestPicsVect.push_back(bestPicsArray[i]);
	}

  //load best pictures into bloom filter
	vector<bool> bestPicsBool = load_bit_vector(bestPicsVect);

  //print bloom filter info at start
  cout<< "********************************" <<endl;
  cout<< "STARTING BLOOM FILTER INFO:" <<endl;
  bloom_info(bestPicsBool);
  
  bool keepGoing = true; //boolean value for while loop
  string answer; //string for user input

  //main user loop
  while (keepGoing == true){
    //check or add to bloom filter
    cout<< "Check if movie is a Best Picture winner or add movie to Best Picture winners [check/add]:" <<endl;
    getline(cin, answer);
    lowercase(answer); //convert input to lowercase
    //user wants to check
    if(answer == "check"){
      cout<< "Enter movie title to see if it is a Best Picture winner:" <<endl;
      getline(cin, answer);
      if(check_vector(bestPicsBool, answer)){ //check if input in bloom filter
        cout<< "Most likely!" <<endl; //if most likely present
      } else {
        cout<< "Nope!" <<endl; //if definitely not present
      }
    //user wants to add
    } else if (answer == "add"){ 
      cout<< "Enter movie title to add to Best Picture winners:" <<endl;
      getline(cin, answer);
      lowercase(answer); //convert input to lowercase
      add_string(bestPicsBool, answer); //add input to bloom filter
      cout<< "Done!" <<endl; 
    }

    //check if user wants to continue loop
    cout<< "Would you like to continue checking or adding? [yes/no]" <<endl;
    getline(cin, answer);
    lowercase(answer);
    if (answer == "yes"){
      keepGoing = true;
    } else {
      keepGoing = false;
    }
  }

  //print bloom filter info on exit
  cout<< "********************************" <<endl;
  cout<< "ENDING BLOOM FILTER INFO:" <<endl;
  bloom_info(bestPicsBool);

	return 0;
}
