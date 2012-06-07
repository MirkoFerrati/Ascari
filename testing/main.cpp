
#include "testcontroller.hpp"

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

}
