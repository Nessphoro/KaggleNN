// Consolepplication3.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
using namespace std;

//http://msdn.microsoft.com/en-us/library/ms229017.aspx
class Data{
public:

	Data(){

	}
	Data(ifstream& i){
		i >> id;
		i.get();
		for (int c = 0; c < 30; c++){
			i >> features[c];
			i.get();
		}
		i >> weight;
		char delim;
		i.get();
		signal = (i.get() == 's');
	}
	void printEverything(){
		cout << id << ' ';
		for (char c = 0; c < 30; c++){
			cout << features[c] << ' ';
		}
		cout << weight << ' ';
		cout << signal << '\n';

	}
	bool getSignal(){
		return signal;
	}
private:
	int id;
	float features[30];
	float weight;
	bool signal;
};
template <class A>
class VectorPair{
public:
	VectorPair(){

	}
	VectorPair(vector<A> a, vector<A> b){
		one = a;
		two = b;
	}
	void PushFirst(A a){
		one.push_back(a);
	}
	void PushSecond(A a){
		two.push_back(a);
	}
	void crossValidate(float percentStart, float percentAmount){
		vector <A> temp;
		vector <A> temp2;
		const int size = one.size();
		for (int i = 0; i < size; i++){
			float percentThrough = ((float)i) / size;
			if (percentThrough >= percentStart&&percentThrough <= percentStart + percentAmount){
				temp.push_back(one.at(i));
			}
			else{
				temp2.push_back(one.at(i));
			}
		}
		const int sizeTwo = two.size();
		for (int i = 0; i < sizeTwo; i++){
			float percentThrough = ((float)i) / sizeTwo;
			if (percentThrough >= percentStart&&percentThrough <= percentStart + percentAmount){
				temp.push_back(two.at(i));
			}
			else{
				temp2.push_back(two.at(i));
			}
		}
		one = temp;
		two = temp2;
	}
	vector <A> getVectorOne(){
		return one;
	}
	vector <A> getVectorTwo(){
		return two;
	}
private:
	vector < A > one;
	vector < A > two;
};

int _tmain(int argc, _TCHAR* argv[])
{
	FILE* sampleFile = fopen("c:/users/gargantuan/kagglenn/data.bin", "rb");
	Data point;
	VectorPair<Data> pair;
	for (int i = 0; i < 250000; i++){
		fread(&point, 132, 1, sampleFile);


		if (point.getSignal()){
			pair.PushFirst(point);
		}
		else{
			pair.PushSecond(point);
		}
	}
	fclose(sampleFile);
	VectorPair<Data> newPair = pair;
	newPair.crossValidate(0.1, 0.2);
	cout << "Finished";
	char c;
	cin >> c;
	return 0;
}







