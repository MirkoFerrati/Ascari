#include "testautomaton.hpp"
#include "testcontroller.hpp"
#include "testdynamic.hpp"
#include "testudpsenderreceiver.hpp"
#include "testencoder.hpp"
#include "testdecoder.hpp"
#include "testClass.h"
#include "testsimulator.hpp"
#include "testrandom.hpp"
#include "time.h"
#include "testagentudpcommunicator.hpp"
// #include "testgeometry.hpp"
#include <forward_list>
/**
 * @author MirkoF
 * Questo progetto e' ancora troppo piccolo per avere un vero framework di unit testing, per adesso 
 * basta e avanza un sistema banale come quello qui sotto: una classe base di test e tutti i test
 * che derivano da quella, e un vettore con i test aggiunti manualmente.
 */

int main(int /*argc*/, char** /*argv*/) {
	srand(time(NULL));
	
	int i=0;
	std::forward_list<double> test;
	for (i=0;i<10;i++)
	  test.insert_after(test.before_begin(),i);
	auto old_pointer=test.before_begin();
	for (auto pointer=test.begin();pointer!=test.end();++pointer)
	{
	  old_pointer++;
	  if (true)//(*pointer>0)
	  {
	      test.erase_after(old_pointer);
	      pointer=old_pointer;
	      break;
	  }
	  
	    
	}
	
	for (auto p:test)
	  cout<<p<<" ";
	cout<<endl;
	
	
	
	vector<pair<testClass*,string> > tests;
	tests.push_back(make_pair(new testRandom(),"random"));
	tests.push_back(make_pair(new testController(),"controllore"));
	tests.push_back(make_pair(new testDynamic(),"dinamica"));
	tests.push_back(make_pair(new testUDPSenderReceiver(),"UDP sender receiver"));
	tests.push_back(make_pair(new testEncoder(),"Encoder"));
	tests.push_back(make_pair(new testAutomaton(),"automaton"));
	tests.push_back(make_pair(new testDecoder(),"Decoder"));
	//tests.push_back(make_pair(new testAgentUDPCommunicator(),"Agent Udp Communicator"));
// 	tests.push_back(make_pair(new testGeometry(),"Geometry"));
	
// 	tests.push_back(make_pair(new testSimulator(),"Simulator"));
	
	for (unsigned int i=0;i<tests.size();i++)
	{
		try 
		{
			cout<<"inizio test "<<tests[i].second<<endl;
			tests[i].first->test();
			cout<<"il test "<<tests[i].second<<" e' andato a buon fine"<<endl;
		}
		catch(const char* e)
		{
			cout<<e<<endl;
		}
		catch (...)
		{
			cout<<"il test "<<tests[i].second<<" e' fallito"<<endl;
			throw;
		}
	}
	
	for (unsigned int i=0;i<tests.size();i++)
	{
		delete(tests.at(i).first);
	}
	
}



