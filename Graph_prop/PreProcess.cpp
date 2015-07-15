#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
using namespace std;

void Tokenize(string line, vector<string>& tokens , string delimiter=" ")
{
        stringstream iss(line);
        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));
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

void preProcess(string inputFilePath, string outputFilePath)
{
	ifstream inf;
	ofstream outf;
	inf.open(inputFilePath.c_str());
	outf.open(outputFilePath.c_str());

	string line;
	int ctr1 = 0;
	int ctr2 = 0;
	while(inf.good())
	{
		ctr1++;
		getline(inf,line);
		vector<string> tokens;
		Tokenize(line, tokens, " \t");
		if ((int)tokens.size() ==0)
		{
			ctr2++;
			//outf<<endl;
			continue;	
		}
		if ((int)tokens.size() !=2)
		{
			cout<< "bad number of tokens in line "<<line<<endl;
			throw 1;
		}
		ctr2++;
		outf<<preProcess(tokens[0])<<"\t"<<tokens[1]<<endl;
		if (ctr1!=ctr2)	
			cout<<ctr1<< "\t" << ctr2<<endl;
	}	

	
	inf.close();
	outf.close();
}

int main(int argc, char** argv)
{
	if (argc!= 4)
	{
		cout<<"usage: PreProcess.o 0 InputFilePath OutputFilePath\n";
		return 1;
	}
	preProcess(argv[2],argv[3]);
}
