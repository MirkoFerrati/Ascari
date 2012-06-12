#include "testautomaton.hpp"
#include "testcontroller.hpp"
#include "testdynamic.hpp"
#include "testudpsenderreceiver.hpp"
#include "testencoder.hpp"
#include "testdecoder.hpp"
#include "testClass.h"

/**
 * @author MirkoF
 * Questo progetto è ancora troppo piccolo per avere un vero framework di unit testing, per adesso 
 * basta e avanza un sistema banale come quello qui sotto: una classe base di test e tutti i test
 * che derivano da quella, e un vettore con i test aggiunti manualmente.
 */

int main(int argc, char **argv) {

	vector<pair<testClass*,string> > tests;

	tests.push_back(make_pair(new testController(),"controllore"));
	tests.push_back(make_pair(new testDynamic(),"dinamica"));
	tests.push_back(make_pair(new testUDPSenderReceiver(),"UDP sender receiver"));
	tests.push_back(make_pair(new testEncoder(),"Encoder"));
	tests.push_back(make_pair(new testAutomaton(),"automaton"));
	tests.push_back(make_pair(new testDecoder(),"Decoder"));
	
	for (unsigned int i=0;i<tests.size();i++)
	{
		try 
		{
			cout<<"inizio test "<<tests[i].second<<endl;
			tests[i].first->test();
			cout<<"il test "<<tests[i].second<<" è andato a buon fine"<<endl;
		}
		catch (...)
		{
			cout<<"il test "<<tests[i].second<<" è fallito"<<endl;
			throw;
		}
	}
	
}



