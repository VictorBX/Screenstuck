#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <sstream>
#include <curl/curl.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <time.h>

using namespace std;

//wait timer
void wait_time(int seconds)
{
    clock_t end_time;
    end_time = clock() + seconds*CLOCKS_PER_SEC;
    while(clock() < end_time) {}
}

//inform curl to use this function when writing data
size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    ostringstream *stream = (ostringstream*)userdata;
    size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}

//checks to see if the file is empty
bool file_empty()
{
    ifstream savein("savedate.txt");
    string temp;
    getline(savein,temp);
    bool fempty=false;
    if(temp.length()== 0)
    {
        //cout << "file empty" << endl;
        fempty = true;
    }
    savein.close();
    return fempty;
}

void title_out()
{
    cout << "                            .          .  " << endl;
    cout << ",-. ,-. ,-. ,-. ,-. ,-. ,-. |- . . ,-. | ," << endl;
    cout << "`-. |   |   |-' |-' | | `-. |  | | |   |< " << endl;
    cout << "`-' `-' '   `-' `-' ' ' `-' `' `-^ `-' ' `" << endl;
    cout << "                         by Victor Barrera" << endl;
    cout << "==========================================" << endl;
}

//Setup the sleep time
unsigned int sleep_time()
{
    unsigned int t_sleep=0;
    cout << "Refresh time (in minutes): ";
    cin >> t_sleep;
    while(t_sleep<1)
    {
        cout << endl << "Please enter >= 1 minutes: ";
        cin >> t_sleep;
    }
    cout << endl;
    return t_sleep;
}

unsigned int intro()
{
    title_out();
    unsigned int total_sleep = 60*sleep_time();
    system("CLS");
    title_out();
    cout << "Searching for an update..." << endl;
    return total_sleep;
}
