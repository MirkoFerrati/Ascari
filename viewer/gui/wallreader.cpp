#include "wallreader.h"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

string FN_ToLower(const string& s)
{
    string str(s);
    for(size_t i = 0; i < str.length(); ++i)
        str[i] = tolower(str[i]);
    return str;
}


WallReader::WallReader(string filename)
{
    cout<<filename<<endl;
    ifstream f;
    f.open(filename.c_str());
    string temp;
    while (!f.eof()){
        getline(f,temp);
        temp.erase (std::remove (temp.begin(), temp.end(), ' '), temp.end());
        if (temp.compare("<wall>")==0)
        {
            wallState w;
            int centerx,centery,width,height;
            string value,name;
            for (int i=0;i<5;i++)
            {
                getline(f,temp);
                temp.erase (std::remove (temp.begin(), temp.end(), ' '), temp.end());
                temp=FN_ToLower(temp);
                value=temp.substr(temp.find_first_of('=')+1,temp.length()-temp.find_first_of('='));
                if (temp.find("name")==0)
                    name=value;
                if (temp.find("centerx")==0)
                    centerx=atoi(value.c_str());
                if (temp.find("centery")==0)
                    centery=atoi(value.c_str());
                if (temp.find("width")==0)
                    width=atoi(value.c_str());
                if (temp.find("height")==0)
                    height=atoi(value.c_str());
            }
            getline(f,temp);
            temp.erase (std::remove (temp.begin(), temp.end(), ' '), temp.end());
            if (temp.compare("</wall>")!=0)
            {
                cout<<"errore, non trovato il tag di chiusura";
            }
            //cout<<height<<width<<centery<<centerx<<endl;
            w.Wall_centerx=centerx;
            w.Wall_centery=centery;
            w.Wall_height=height*2;
            w.Wall_width=width*2; //cambia la notazione, per chi scrive è la larghezza/2, per le QT è larghezza intera
            w.name=name;
            walls.push_back(w);
        }
    }

}

vector<wallState> WallReader::getWalls()
{
    return walls;
}
