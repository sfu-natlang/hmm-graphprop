#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <map>
#include <math.h>
#include <time.h>
#include <set>

using namespace std;


const int treshold = 50;
const int SecondTreshold = 500;

typedef string (*FeatureFunction)(vector<string>);
map<string, FeatureFunction> FeatureFunctions;

void Tokenize(string line, vector<string>& tokens , string delimiter=" ")
{
        stringstream iss(line);
        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));
}


const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    
    return buf;
}

map<int,set<string>> suffixes;

void ReadSuffixes(string SuffixFilePath)
{
	ifstream inf;
	inf.open(SuffixFilePath.c_str());

	string line;
	while(inf.good())
	{
		getline(inf,line);
		vector<string> tokens;
		Tokenize(line,tokens,"\t ");
		if ((int)tokens.size()==0)
			continue;
		string s = tokens[0];
		int l = (int)s.size();
		suffixes[l].insert(s);
	}
	inf.close();
}

string function_suffix(vector<string> params)
{
	if ((int)params.size()!=1)
	{
		cout<<"incorrect number of parameters for suffix fuction"<<endl;
		throw 1;
	}
		
	string word = params[0];

	for (auto itr=suffixes.crbegin(); itr!=suffixes.crend(); ++itr)
	{
		int sl= (itr->first);
		//cout << sl<<endl;
		int wl= (int)word.size();
		if (wl<=sl)
			continue;
		string suffix = word.substr(wl-sl,sl);
		
		if ((suffixes[sl].find(suffix))!=(suffixes[sl]).cend()) // if the word has a suffix
			return "suffix: "+ suffix;
	}
	return "suffix: \"-\"";
}

string preProcess(string word)
{
	
	std::transform(word.begin(), word.end(), word.begin(), ::tolower);	
	
	//Change it if it's a number;
	 int IntSize = word.size();
         int FloatSize = word.size();

	for (auto itr=word.begin();itr!=word.end(); ++itr)
	{
		char c = *itr;
		
		if (c=='1')
                {
                        IntSize--;
                        FloatSize--;
                }
		else if (c=='2')
		{
                 	IntSize--;
			FloatSize--;
		}
                else if (c=='3')
                {
                        IntSize--;
                        FloatSize--;
                }
                else if (c=='4')
                {
                        IntSize--;
                        FloatSize--;
                }
                else if (c=='5')
                {
                        IntSize--;
                        FloatSize--;
                }
                else if (c=='6')
                {
                        IntSize--;
                        FloatSize--;
                }
                else if (c=='7')
                {
                        IntSize--;
                        FloatSize--;
                }
                else if (c=='8')
                {
                        IntSize--;
                        FloatSize--;
                }
                else if (c=='9')
                {
                        IntSize--;
                        FloatSize--;
                }
                else if (c=='0')
                {
                        IntSize--;
                        FloatSize--;
                }
                else if (c==' ')
                {
                        IntSize--;
                        FloatSize--;
                }
                else if (c==',')
                {
                        IntSize--;
                        FloatSize--;
                }
                else if (c=='.')
                {                        
                        FloatSize--;
                }                                	
	}
	


	if (FloatSize==0)
		return "FLOAT";
	
	if (IntSize==0)
                return "INTEGER";

	return word;
}

string CenterWord(string ngram)
{
	vector<string> tokens;
	Tokenize(ngram, tokens);
	int n = (int)tokens.size();
	if ((n%2)!=1)
	{
		cout<<"badsize"<<endl;
		throw 1;

	}
	
	string center = tokens[(n-1)/2];
	return center;
}

map<string,int> labels;
int label_ctr = 0;
float Similarity_treshold = 0;

map<string,int> Global_Feature_cnts;
map<string,float> Global_Feature_probs;

void Read_Labels(string FilePath)
{

	if (labels.find("<s>") == labels.end())
        {
	        labels["<s>"]=label_ctr;
                label_ctr++;
        }

	if (labels.find("</s>") == labels.end())
        {
                labels["</s>"]=label_ctr;
                label_ctr++;
        }

        ifstream f;
        f.open(FilePath.c_str());
        while (f.good())
        {
                string line;
                getline(f, line);
                vector<string> tokens;
                Tokenize(line,tokens);
                int size = (int)tokens.size();
                if (size > 0) 
                {
                        string label = tokens[size-1];
                        if (labels.find(label) == labels.end())
                        {
                                labels[label]=label_ctr;
                                label_ctr++;
                        }
                }
        }
}

class Features_Template
{
        public:
                int N; // Context length
                vector<vector<int> > FeatureTemplates_Vector; // Just the reqular ones for now; We are not implementing functions for now
		vector<string> FeatureNames;
                int FeatureVectorSize;

        Features_Template(string FeaturesTemplateFilePath)
        {
                FeatureVectorSize = 0;
                ifstream f;
                f.open(FeaturesTemplateFilePath.c_str());
                int max_=0;
                while (f.good())
                {
                        string line;
                        vector<int> featureTemplate;
                        getline(f,line);
                        vector<string> tokens;
                        Tokenize(line, tokens);
                        if ((int)tokens.size()<2)
                                continue;
                        for (int i=1; i< (int)tokens.size(); i++)
                        {
                                int position = atoi(tokens[i].c_str());
                                featureTemplate.push_back(position);
                                if (abs(position)> max_)
                                        max_ = abs(position);
                        }
			FeatureNames.push_back(tokens[0]);
                        FeatureTemplates_Vector.push_back(featureTemplate);
                        FeatureVectorSize++;
                }
                f.close();
                N = 2* max_+1;

        }

        void  GetFeatures(string Context[], int eptr, string features[], int n) // features is the output of the function
        {
                int center = (eptr+((N-1)/2))%N;
                for (int i=0; i<FeatureVectorSize; i++)
                {
			string FeatureName = FeatureNames[i];
			int FNameSize = (int)FeatureNames[i].size();			
			if (FeatureName.find("function_")!=string::npos) // if it's a function feature
			{
				vector<string> params ;
				for (int j=0; j< (int) FeatureTemplates_Vector[i].size(); j++)
                                {
                                        int pos = (N+center + FeatureTemplates_Vector[i][j])%N;
                                        params.push_back(Context[pos]);
                                }
				features[i]= FeatureFunctions[FeatureName](params);
			}
			else // if this is not a function feature
			{				
                        	string feature = FeatureName+": ";
                	        for (int j=0; j< (int) FeatureTemplates_Vector[i].size(); j++)
        	                {
	                                int pos = (N+center + FeatureTemplates_Vector[i][j])%N;
					feature += Context[pos]+" ";
                        	}
                        	features[i] = feature;
			}
                }
	}
};

int Max_neighbors_cnt;

class graph
{

	class node 
	{
        	public:
			int ngram_type_cnt;
			int* Label_Count;
                	float* Label_Dist;
			int Neighbors_cnt;
			float min_similarity;
			map<string,int> Feature_cnts;
			int Feature_cnts_Values_Sum;
			map<string,float> Feature_pmi;// pointwise mutual information of each feature and the ngram type
                	string* Neighbor_CenterWords;
                	string* Neighbor_ngrams;
			float* Neighbor_Similarity;

		node()
        	{
			Neighbor_CenterWords = new string[Max_neighbors_cnt];
			Neighbor_ngrams = new string[Max_neighbors_cnt];
                        Neighbor_Similarity = new float[Max_neighbors_cnt];
			Label_Count = new int[label_ctr];
			Label_Dist = new float[label_ctr];
                	for (int i=0; i< label_ctr; i++)
                        	Label_Count[i] = Label_Dist[i] = 0;
			Neighbors_cnt = 0; 
			min_similarity = 10000;
			ngram_type_cnt = 0; 
        	}

		void Set_Feature_pmis()
		{
			//cout << "log(2):" << log(2) << endl;
			char c;
			if (ngram_type_cnt <=0)
			{
				cout << "This shouldn't have happened!" <<endl;
				cin >> c;
			}

			int Feature_cnts_Values_Sum = 0;
			for(auto itr= Feature_cnts.begin(); itr != Feature_cnts.end(); itr++)
				Feature_cnts_Values_Sum += itr->second;
				
			if (Feature_cnts_Values_Sum <=0)
				return;

			for(auto itr= Feature_cnts.begin(); itr != Feature_cnts.end(); itr++)
			{
				if (Global_Feature_cnts.find(itr->first)!=Global_Feature_cnts.end())
				{
					// Todo: make sure that this computation is correct. 
					float p_Feature_Given_ngram = (float)itr->second/(float)Feature_cnts_Values_Sum;
					Feature_pmi[itr->first]=log(p_Feature_Given_ngram/Global_Feature_probs[itr->first]); // pmi(x,y)= log(p(x|y)/p(x)) x being a feature and y an ngram type
				}
				else
					cout << "not found in the global_features!!!!! ";
			}
		}		

		float Similarity(node other_node) // computes cosine similarity of pmi vectors
		{
			//cout << "similarity:";
			float InnerProduct = 0;
			// VectorSize
			int sum = 0;
			float thisVectorSize = 0;
			for(auto itr= Feature_pmi.begin(); itr != Feature_pmi.end(); itr++)
                        {
				float f=(itr->second);
				sum+= f*f; 
			}
			thisVectorSize = sqrt(sum);
			
			float otherVectorSize = 0;
			sum = 0;
			for(auto itr= other_node.Feature_pmi.begin(); itr != other_node.Feature_pmi.end(); itr++)
                        {
                                float f2=(itr->second);
                                sum+= f2*f2;
                        }
                        otherVectorSize = sqrt(sum);



			for(auto itr= Feature_pmi.begin(); itr != Feature_pmi.end(); itr++)
			{
				if (other_node.Feature_pmi.find(itr->first) != other_node.Feature_pmi.end())
				{
					float f = (itr->second);
					string key = itr->first;
					float f2 = other_node.Feature_pmi[key];
					InnerProduct += f*f2;			
					
				}
			}
			//cout << sum << endl;

			float sim = 0;
			if (InnerProduct > 0)
				sim = InnerProduct/(thisVectorSize*otherVectorSize);

			return sim;
		}
		
		void Construct_Edge(node other_node, string other_CenterWord, string other_ngram)
		{
			float Candidate_Similarity = Similarity(other_node);
			
			if (Candidate_Similarity <= Similarity_treshold )
				return;
			int min_neighbor_index = -1;
			if (Neighbors_cnt < Max_neighbors_cnt)
			{
				min_neighbor_index = Neighbors_cnt;
                                Neighbors_cnt++;
			}
			else if (min_similarity < Candidate_Similarity) // this means we can improve(increase) min_similarity by replacing one element with the candidate. 
			{
				for (int i=0; i< Neighbors_cnt; i++)
                                	if (Neighbor_Similarity[i] == min_similarity)
                                        	min_neighbor_index = i;

			}

			
			if (min_neighbor_index!=-1)
			{
				//cout << "min_neighbor_index:" << min_neighbor_index << endl;
				Neighbor_ngrams[min_neighbor_index] = other_ngram;
				Neighbor_CenterWords[min_neighbor_index] = other_CenterWord;
				Neighbor_Similarity[min_neighbor_index] = Candidate_Similarity;
				
				// updating min_similarity
				min_similarity = 1000;
				for (int i=0; i< Neighbors_cnt; i++)
                                        if (Neighbor_Similarity[i] < min_similarity)
                                                min_similarity = Neighbor_Similarity[i];				
				//cout << "Edge was added!\tneighbors_cnt: " << Neighbors_cnt << endl;
			}
		}
	};

	map < string, map<string, node> > nodes; //data structure: centerword->ngram->information
	map < string, int > VertexMap;
	int VerticesSize;
	map <string, set<string>> FeatureInstanceMap; // For any feature instance (ex. trigram: a b c) keeps the set of all ngrams(i.k.a vertices) that have that feature. 
	public:
	//int Max_neighbors_cnt; 
	Features_Template* FT;
	graph(string Labeled_Data_File_Path, string Unlabeled_Data_File_Path, int n, Features_Template* Template) //  n is for the n-gram and K is for the K nearest neighbors
	{
		FT = Template;
		VerticesSize = 0;
		
		Construct_Nodes(Labeled_Data_File_Path, Unlabeled_Data_File_Path, n);	
		cout<<"Constructing nodes finished in " << currentDateTime() << std::endl;
		Construct_Edges();
	}

	void MakeNew_nGram_And_Features(string Context[], string word, string Context_labels[], string label, int n, int& endptr, string& CenterWord, string& ngram, string& CenterLabel, bool add_labels, string FeatureVector[])
	{
		int N= FT->N;
		if (add_labels==false)
			label = "-";
        	Context[endptr]=word;
		Context_labels[endptr]=label;
        	endptr++;
        	endptr %= N;

		ngram = "";
        	for (int i=0; i<n; i++)
                	ngram+= Context[(endptr+((N-n)/2)+i)%N] + " ";

		if (VertexMap.find(ngram)==VertexMap.cend())
		{
			VertexMap[ngram]=VerticesSize;
	                VerticesSize++;
        	        //cout<<VerticesSize<<endl;
		}
		

		int centerPosition = (endptr+((N-1)/2))%N;
		CenterWord = Context[centerPosition];
		CenterLabel = Context_labels[centerPosition];
		FT->GetFeatures(Context, endptr, FeatureVector, n);
	}

	void Initialize_Context(string Context[], string Context_labels[])
	{
		int N = FT->N;
		
		for (int ctr=0; ctr<N; ctr++)
		{
                        Context[ctr] = "<s>";
			Context_labels[ctr] = "<s>";
		}
	}

	void UpdateNode(string CenterWord, string ngram, string label, bool add_labels, string FeatureVector[])
	{
		// Adding the ngram type if it's seen for the first time ...
                if (nodes.find(CenterWord) != nodes.end())		
                	if (nodes[CenterWord].find(ngram) != nodes[CenterWord].end())
                	{
 	   			// This space was deliberately left blank since I found it easier to code the else part this way. 
                	}
                else
                {      
			node myNode; 
         	       	nodes[CenterWord][ngram]= myNode;
			//VertexMap[ngram]=VerticesSize;
			//VerticesSize++;
			//cout<<VerticesSize<<endl;
                }
	
		nodes[CenterWord][ngram].ngram_type_cnt++;

		for (int i=0; i< FT-> FeatureVectorSize; i++)
		{
			//cout<< "this is an example of FeatureVector[i]: "<< FeatureVector[i]<<endl;
			nodes[CenterWord][ngram].Feature_cnts[FeatureVector[i]]++;
			Global_Feature_cnts[FeatureVector[i]]++;
			FeatureInstanceMap[FeatureVector[i]].insert(ngram);
		}
		// Increasing the label's counter if we are in labeled data 
		if (add_labels)
			nodes[CenterWord][ngram].Label_Count[labels[label]]++;
	}

	void Create_Nodes(string Data_File_Path, int n, bool add_labels) // n is for the n-gram types, N is the length of the context
	{
		int N = FT-> N;
		//cout << " Create_Nodes N: "<< N <<endl;

		int FVSize = FT-> FeatureVectorSize;
		if ((N%2 ==0) || (n%2 ==0) || ( n >= N))
		{
			cout <<"Parameter Choice Error: n and N should be odd and n should be smaller than N! n:" << n << " N:" <<N <<endl;
			return;
		}

		ifstream f;
                f.open(Data_File_Path.c_str());

		string Context[N];
		string Context_labels[N];
                Initialize_Context(Context, Context_labels);

                bool end_of_the_sentence_was_seen = true; // we need this variable for ignoring multiple empty lines between sentences and also for adding the last </s> signs for the last sentence in case there was no empty line after it
      		int endptr = N-1; // we use this pointer to retrieve the current n-gram and features from Context instead of shifting words to the left and adding the new word to the end of the array, we move the pointer to the beginning
      
		string CenterWord, ngram, CenterLabel;
		string FeatureVector[FVSize];

	        while (f.good())
                {
			string line;
                        getline(f, line);
			vector<string> tokens;
                        Tokenize(line,tokens);
			int size = (int)tokens.size();
                        if (size != 0)
                        {
				string word = preProcess(tokens[0]);
                                end_of_the_sentence_was_seen = false; 
				//std::cout << "started making new ngram and features in " << currentDateTime() << std::endl;
				MakeNew_nGram_And_Features(Context, word, Context_labels, tokens[size-1], n, endptr, CenterWord, ngram, CenterLabel, add_labels, FeatureVector);
				//std::cout << "finished making new ngram and features in " << currentDateTime() << std::endl;

				//std::cout << "started updating node in " << currentDateTime() << std::endl;
				UpdateNode(CenterWord,ngram,CenterLabel, add_labels, FeatureVector);
				//std::cout << "finished updating node in " << currentDateTime() << std::endl;
                        }
                        else if (end_of_the_sentence_was_seen == false) // ignoring the multiple empty lines between sentences
                        {
                                end_of_the_sentence_was_seen = true;
                                for (int ctr=0; ctr<N-1; ctr++)
				{
					MakeNew_nGram_And_Features(Context, "</s>", Context_labels, "</s>", n, endptr, CenterWord, ngram, CenterLabel, add_labels,FeatureVector);
                                	UpdateNode(CenterWord,ngram,CenterLabel, add_labels, FeatureVector);
				} 
				Initialize_Context(Context, Context_labels);
                        }
                }

                if (end_of_the_sentence_was_seen == false) // Takes care of the closing of the last sentence if there was no empty line after that
                        for (int ctr=0; ctr<N-1; ctr++)
			{
				MakeNew_nGram_And_Features(Context, "</s>", Context_labels, "</s>", n, endptr, CenterWord, ngram, CenterLabel, add_labels,FeatureVector);
                                UpdateNode(CenterWord,ngram,CenterLabel, add_labels, FeatureVector);

			}
		f.close();
	}

	void Print_Graph(string vertexMapFilePath, string GraphStructureFilePath)
	{
		ofstream of_vm;
		ofstream of_gs;
		of_vm.open(vertexMapFilePath.c_str());
		of_gs.open(GraphStructureFilePath.c_str());
		
		for (auto it1 = nodes.begin(); it1!= nodes.end(); it1++)
                { 
                        map<string, node>* temp = &it1->second;
                        for (auto it2 = temp->begin(); it2 != temp->end(); it2++)
                        {	
				string ngram = it2->first;
				int nodeNumber = VertexMap[ngram];
				of_vm<<nodeNumber<<"\t"<<ngram<<endl;
				of_gs<<nodeNumber<<"\t";
				for (int i=0; i< it2->second.Neighbors_cnt; i++)
				{
					string neighbor_ngram = it2->second.Neighbor_ngrams[i];
                                        of_gs<< VertexMap[neighbor_ngram]<<": "<<it2->second.Neighbor_Similarity[i]<<"\t";
				}
				of_gs<<endl;
			}
		}

		of_vm.close();
		of_gs.close();
	}
	
	void Print_Graph_For_Debug()
	{
		for (auto it1 = nodes.begin(); it1!= nodes.end(); it1++)
		{ 
			map<string, node>* temp = &it1->second;
			for (auto it2 = temp->begin(); it2 != temp->end(); it2++)
			{				
				cout << it2->first<<":"<<endl;
				cout << "ngram_type_cnt:\t\t\t";
				cout<< it2->second.ngram_type_cnt<<"\t";
                                cout<<"\n";
 
				cout <<"Label Distribution: \t\t\t";
                                for (int i=0; i< label_ctr; i++)
                                        cout<< it2->second.Label_Dist[i]<<"\t";
                                cout<<"\n";
				cout <<"Label Counts: \t\t\t";
                                for (int i=0; i< label_ctr; i++)
                                        cout<< it2->second.Label_Count[i]<<"\t";
                                cout<<"\n";				
				cout << "#Neighbors:" << it2->second.Neighbors_cnt << endl; 
				cout<<"Neighbors: \t\t\t";
				for (int i=0; i< it2->second.Neighbors_cnt; i++)
					cout<< it2->second.Neighbor_ngrams[i]<<"\t";
				cout <<"\n";
				cout<<"Neighbors' weights: \t\t\t";
                                for (int i=0; i< it2->second.Neighbors_cnt; i++)
                                        cout<< it2->second.Neighbor_Similarity[i]<<"\t";
                                cout <<"\n";

				cout<<"Features: \n";
				map<string, int> NodeFeatures = it2->second.Feature_cnts;
				for (auto it3 = NodeFeatures.begin(); it3!= NodeFeatures.end(); it3++)
					cout<< it3->first << "\t\t\t:" << it3->second << endl;
				cout<<"Feature_mpis: \n";
                                map<string, float> NodeFeature_pmi = it2->second.Feature_pmi;
                                for (auto it3 = NodeFeature_pmi.begin(); it3!= NodeFeature_pmi.end(); it3++)
                                        cout<< it3->first << "\t\t\t:" << it3->second << endl;
				cout <<"-------------------------------------------------\n\n";
			}
			
		}
	}
	
	void Get_Label_Distributions()
	{
		for (auto it1=nodes.begin(); it1!= nodes.end(); it1++)
		{
			map<string, node> temp = it1->second;
			for (auto it2=temp.begin(); it2!= temp.end(); it2++)
			{
				int sum=0;
				for (int i=0; i<label_ctr; i++)
					sum += it2->second.Label_Count[i];
				if (sum==0)
					continue;
				for (int i=0; i<label_ctr; i++)
                                	it2->second.Label_Dist[i] += (float)(it2->second.Label_Count[i])/(float)sum;
			}
		}
	}

	void Construct_Edges()
	{
		ofstream Skipped;
		Skipped.open("skipped_vertices.txt");

		/*for (auto feature_itr=FeatureInstanceMap.cbegin(); feature_itr!= FeatureInstanceMap.cend(); ++feature_itr)
			if ((feature_itr->second).size()>treshold)
				cout<<feature_itr->first<<"\t"<<(feature_itr->second).size()<<endl;*/

		int nodectr=0;

		for (auto node1_center_itr=nodes.begin(); node1_center_itr!= nodes.end(); node1_center_itr++)
                {
			map<string, node>* temp1 = &node1_center_itr->second;
                        for (auto node1_ngram_itr=temp1->begin(); node1_ngram_itr!= temp1->end(); node1_ngram_itr++)
                        {	
				cout<<nodectr++<<"\t"<< currentDateTime() <<"\t" ;
		
				/*for (auto node2_center_itr=nodes.begin(); node2_center_itr!= nodes.end(); node2_center_itr++)
		                {
                		        map<string, node>* temp2 = &node2_center_itr->second;
                        		for (auto node2_ngram_itr=temp2->begin(); node2_ngram_itr!= temp2->end(); node2_ngram_itr++)
                        		{
						string other_CenterWord = node2_center_itr->first;
						string other_ngram = node2_ngram_itr->first;
						if (node1_ngram_itr->first != node2_ngram_itr->first)
						{							
							node1_ngram_itr->second.Construct_Edge(node2_ngram_itr->second,other_CenterWord,other_ngram);
						}
					}
				}*/
				//instead of considering all pairs of nodes, we consider (a,b)'s where a and b have some featureInstance in common.
				

				map<string,int> otherNodeCandidates;
				node* thisNode = &(node1_ngram_itr->second);
				for (auto feature_itr=thisNode->Feature_pmi.cbegin(); feature_itr!=thisNode->Feature_pmi.cend(); ++feature_itr)
				{
					string fKey = feature_itr->first;					
					
					if (fKey.substr(((int)fKey.size())-3,3)=="\"-\"") //if the featureInstance is the absence of a feature for example
						continue;
					if (FeatureInstanceMap[fKey].size()>treshold)
						continue;
					for (auto set_itr=FeatureInstanceMap[fKey].cbegin(); set_itr!=FeatureInstanceMap[fKey].cend(); ++set_itr)
						otherNodeCandidates[*set_itr]++;					
				}
				int count =0;
				if ((int)otherNodeCandidates.size()>SecondTreshold)
				{
					cout<< "skipped " << node1_center_itr->first << " because of too many candidates: " << (int)otherNodeCandidates.size()  << endl;
					Skipped<< node1_center_itr->first << endl;
					continue;
				}
				for (map<string,int>::iterator c_itr=otherNodeCandidates.begin(); c_itr!=otherNodeCandidates.end(); ++c_itr)
				{
					string other_ngram = c_itr->first;
					string other_CenterWord = CenterWord(other_ngram);
					//cout<<other_ngram<<"\t"<<other_CenterWord<<endl;
					if (other_ngram == node1_ngram_itr->first) // if this_ngram == other_ngram
						continue;
					node1_ngram_itr->second.Construct_Edge((nodes[other_CenterWord])[other_ngram],other_CenterWord,other_ngram);
				
					//if ((c_itr->second)>1)
						count++;
				}
				cout<< node1_ngram_itr->first  << "\t" << currentDateTime() << "\t number of candidates:" << count << "\n" ;			
			}
		}
		Skipped.close();
	}

	void Compute_PMI_FeatureVectors()
	{	
		for (auto it1=nodes.begin(); it1!= nodes.end(); it1++)
                {
                        map<string, node> temp = it1->second;
                        for (auto it2=temp.begin(); it2!= temp.end(); it2++)
	               		nodes[it1->first][it2->first].Set_Feature_pmis();
		}
	}

	void Construct_Nodes(string Labeled_Data_File_Path, string Unlabeled_Data_File_Path, int n)
	{
		std::cout << "Creating nodes started at " << currentDateTime() << std::endl;

		Create_Nodes(Labeled_Data_File_Path, n, true);
		Create_Nodes(Unlabeled_Data_File_Path, n, false);

		std::cout << "Creating nodes finished at " << currentDateTime() << std::endl;
		cout<< "Number of Vertices: "<<VerticesSize<<endl;
		
		std::cout << "Get Label Distributions started at " << currentDateTime() << std::endl;

		Get_Label_Distributions(); // will normalize the label counts
		std::cout << "Get Label Distribution finished at " << currentDateTime() << std::endl;

		std::cout << "Get Global Features info started at " << currentDateTime() << std::endl;

		int Global_Feature_Sum_Of_All_Counts = 0;
		for (auto it= Global_Feature_cnts.begin(); it != Global_Feature_cnts.end(); it++)
			Global_Feature_Sum_Of_All_Counts += it->second;
		for (auto it= Global_Feature_cnts.begin(); it != Global_Feature_cnts.end(); it++)
			Global_Feature_probs[it->first] = (float) it->second / (float) Global_Feature_Sum_Of_All_Counts;
		
		std::cout << "Get Global Features info finished at " << currentDateTime() << std::endl;
		//cout << "Global_Feature_Sum_Of_All_Counts:" << Global_Feature_Sum_Of_All_Counts << endl;	
		//
		std::cout << "Compute PMI FeatureVectors started at " << currentDateTime() << std::endl;

		Compute_PMI_FeatureVectors();

		std::cout << "Compute PMI FeatureVectors finished at " << currentDateTime() << std::endl;
	}
};

void PrintLabels()
{
        for (auto it = labels.begin(); it != labels.end(); it++)
                cout << it->first << ":" << it->second << "\t";
	cout<<endl;
}

void PrintGlobalFeatures()
{
        for (auto it= Global_Feature_cnts.begin(); it != Global_Feature_cnts.end(); it++)
                cout << it->first << ": " << it->second << " -> " << Global_Feature_probs[it->first] << endl;
	cout<<endl;
}


int main(int argc, char** argv)
{
	if (argc<11)
	{
		cout << "Parameters: Labeled_File_Path, Unlabeled_File_Path, TemplateFilePath, n(in n_gram), k(maximum number of neighbors), OutputVertexMapFilePath, OutputGraphStructureFilePath, debug_mode(0/1) fuffixesFilePath" << endl;
		return 1; 
	}

	FeatureFunctions["function_suffix"]= function_suffix;
	
	ReadSuffixes(argv[10]);
	Read_Labels(argv[2]);
	Features_Template FT(argv[4]);
	Max_neighbors_cnt = atoi(argv[6]);
	int n = atoi(argv[5]);
	std::cout << "currentDateTime()=" << currentDateTime() << std::endl;
	graph g(argv[2],argv[3],n, &FT);
	std::cout << "currentDateTime()=" << currentDateTime() << std::endl;
	g.Print_Graph(argv[7],argv[8]); 

	int debug_mode = atoi(argv[9]);
	if (debug_mode)
	{
		g.Print_Graph_For_Debug();
		PrintLabels();
        	PrintGlobalFeatures();
	}
	
	
	return 0;
}

