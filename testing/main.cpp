
#include "testcontroller.hpp"
#include "testdynamic.hpp"
#include "testudpsenderreceiver.hpp"

int main(int argc, char **argv) {


    try {
        cout<<"inizio test del controllore"<<endl;
        testController c;
        c.test();
        cout<<"il test del controllore è andato a buon fine"<<endl;
    }
    catch (...)
    {
        cout<<"il test del controllore è fallito"<<endl;
        throw;
    }

try {
        cout<<"inizio test della dinamica"<<endl;
        testDynamic d;
        d.test();
        cout<<"il test della dinamica è andato a buon fine"<<endl;
    }
    catch (...)
    {
        cout<<"il test della dinamica è fallito"<<endl;
        throw;
    }
    
try {
        cout<<"inizio test della comunicazione udp"<<endl;
        testUDPSenderReceiver u;
        u.test();
        cout<<"il test della comunicazione udp è andato a buon fine"<<endl;
    }
    catch (...)
    {
        cout<<"il test della comunicazione udp è fallito"<<endl;
        throw;
    }
}



