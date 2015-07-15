#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <iterator>

using namespace std;


void Tokenize(string line, vector<string>& tokens , string delimiter=" ")
{
        stringstream iss(line);
        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));
}


map<string,map<string,int>> Read_Cooccurrences(char* RefFilePath, char* ModelOutputPath)
{

	map<string, map<string, int>> co_occurs;

        ifstream ref;
        ref.open(RefFilePath);
        ifstream modelf;
        modelf.open(ModelOutputPath);

        string line;
        while (ref.good())
        {
                getline(ref, line);
		//cout<<line<<endl;

                vector<string> tokens;
                Tokenize(line, tokens, "\t");
                
                string refTag= "";
                int Size = (int) tokens.size();
                if (Size != 0 )
                        refTag = tokens[Size-1];
                        
                getline(modelf, line);
		//cout << line << endl;
                vector<string> tokens2;
                Tokenize(line, tokens2, "\t");

                string modelTag= "";
                Size = (int) tokens2.size();
                if (Size != 0 )
                        modelTag = tokens2[Size-1];

                if ((refTag=="") || (modelTag==""))
                {
                        //cout << "Empty tags!!!"<<endl;
                        continue;
                }
                else
		{                
			(co_occurs[refTag])[modelTag]++;                
		}
        }

        ref.close();
        modelf.close();
	return co_occurs;
}

map<string, string> Greedy_mapping(map<string,map<string,int>> co_occurs)
{
	map<string, string> mapping;
	map<string, bool> taken;
	map<int, vector<string>> sorted_Frequencies;

	vector<string> NotAssigned;
	
	for (auto itr1= co_occurs.cbegin(); itr1!= co_occurs.cend(); ++itr1)
	{
		int total = 0;
		map<string, int> current = itr1->second;
		for (auto itr2= current.cbegin(); itr2!= current.cend(); ++itr2)
			total+= (itr2->second);

		sorted_Frequencies[total].push_back(itr1->first);		
	}

	for (auto itr= sorted_Frequencies.crbegin(); itr!= sorted_Frequencies.crend(); ++itr)
	{
		vector<string> currentTags = itr->second;
		for (auto itr1= currentTags.cbegin(); itr1!= currentTags.cend(); ++itr1)
		{
			string RefTag = *itr1;
			map<string, int> ModelTags = co_occurs[RefTag];
			int max = 0;
			string argmax = "";
			for (auto itr2 = ModelTags.cbegin(); itr2!= ModelTags.cend(); ++itr2)
				if (!(taken[itr2->first]) && (itr2->second > max))
				{
					max = itr2->second;
					argmax = itr2->first;
				}
			taken[argmax]=true;
			if (argmax != "")
				mapping[argmax]=RefTag;
			else
				NotAssigned.push_back(RefTag);
		}
	}

	auto itr0 = taken.cbegin();
	for (auto itr=NotAssigned.cbegin(); itr!= NotAssigned.cend(); ++itr)
	{
		while((itr0!=taken.cend())&&(itr0->second))
			++itr0;
		if (itr0!= taken.cend()) // means that it's not taken
			mapping[itr0->first] = *itr;
		else
			break;
	}
		

	return mapping; 
}

float GetAccuracy(char* RefFilePath, char* ModelOutputPath, map<string,string> TagMaps)
{
	ifstream ref;
        ref.open(RefFilePath);
        ifstream modelf;
        modelf.open(ModelOutputPath);

	int totalRecordsNum = 0;
	int correctNum = 0;

        string line;
        while (ref.good())
        {
                getline(ref, line);
                vector<string> tokens;
                Tokenize(line, tokens, "\t");

                string refTag= "";
                int Size = (int) tokens.size();
                if (Size != 0 )
                        refTag = tokens[Size-1];

                getline(modelf, line);
                vector<string> tokens2;
                Tokenize(line, tokens2, "\t");

                string modelTag= "";
                Size = (int) tokens2.size();
                if (Size != 0 )
                        modelTag = tokens2[Size-1];

                if ((refTag=="") || (modelTag==""))
                {
                        //cout << "Empty tags!!!"<<endl;
                        continue;
                }
                else
		{
			totalRecordsNum++;
			if (refTag == TagMaps[modelTag])
				correctNum++;
		}                        
        }

        ref.close();
        modelf.close();
	//cout << correctNum << " out of "<< totalRecordsNum << endl;
	if (totalRecordsNum ==0)
		return 0;

	return ((float)correctNum)/((float)totalRecordsNum);
}

void print_Cooccurrences(map<string, map<string,int>> arg)
{
	cout << "1.5\n";
	for (auto itr=arg.cbegin();  itr!= arg.cend(); ++itr)
	{
		cout << itr->first <<"\t";
		map<string, int> current = itr->second;

		for (auto itr2= current.cbegin(); itr2!= current.cend(); ++itr2)
			cout << itr2->first <<":"<<itr2->second<<"\t";

		cout <<endl;
	}
}


void print_TagMaps(map<string, string> TagMaps)
{
	cout <<"TagMaps: \n";
	for (auto itr=TagMaps.cbegin(); itr!= TagMaps.cend(); ++itr)
		cout <<itr->first <<"->" << itr->second << endl;
}

int main(int argc, char** argv)
{

/*	if (argc != 4)
	{
		cout <<"Usage: Indentify_Tags.o 0 RefrenceFilePath ModelOutputPath"<<endl;
		return 1;
	}*/

	char* argv_2 = "/cs/natlang-projects/users/Golnar/Documents/Academic/Code/Graph-Subramanya/HMM/refLabels.txt";
	char* argv_3 = "/cs/natlang-projects/users/Golnar/Documents/Academic/Code/Graph-Subramanya/HMM/modelLabels.txt";

	map<string,map<string, int>> co_occurs = Read_Cooccurrences(argv_2,argv_3);

	//cout << "hello there 1\n";
	//print_Cooccurrences(co_occurs);	

	//cout << "2\n";
	map<string,string> TagMaps = Greedy_mapping(co_occurs);

	//cout << "3\n";
	//print_TagMaps(TagMaps);

	
	float Accuracy = GetAccuracy(argv_2, argv_3, TagMaps);

	cout << "Accuracy=" << Accuracy << endl;
	return 0;
	
}

