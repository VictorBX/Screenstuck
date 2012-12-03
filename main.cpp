#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <sstream>
#include <curl/curl.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <windows.h>


using namespace std;

//inform curl to use this when writting data
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
        cout << "file empty" << endl;
        fempty = true;
    }
    savein.close();
    return fempty;
}

int main(void)
{
    //time for sleep
    unsigned int t_sleep = 0;
    cout << "Refresh time (in minutes): ";
    cin >> t_sleep;
    while(t_sleep==0)
    {
        cout << endl << "Please enter >= 1 minutes: ";
        cin >> t_sleep;
    }
    cout << endl;
    unsigned int total_sleep = 60000*t_sleep;

    while(true)
    {
    //curl initialize
    CURL *curl;
    CURLcode res;

    //location at which xml is saved
    ofstream fileout("xmldata.txt");

    //location of the publication date on xml
    int pubdate = 0;
    size_t loc;

    curl = curl_easy_init();
    if(curl)
    {
        ostringstream stream;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
        curl_easy_setopt(curl, CURLOPT_URL, "http://www.mspaintadventures.com/rss/rss.xml");

        //Perform the request, res will get the return code
        res = curl_easy_perform(curl);

        //Check for errors
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        string output = stream.str();
        fileout << output << std::endl;
        loc = output.find("pubDate");
        pubdate = int(loc);
        string save = output.substr(pubdate+8,31);

        //if the saveout file is empty, then write the date of the
        //newest update in the saveou file
        if(file_empty()==true)
        {
            //cout << "input to empty file" << endl;
            ofstream saveout("savedate.txt");
            saveout << output.substr(pubdate+8,31) << endl;
            saveout.close();
        }

        //Using the new xml file in fileout, check the new date of
        //the latest update and compare it to the date in saveout
        //every minute. If different, theer is an update.
        ifstream savecomp("savedate.txt");
        string compare;
        getline(savecomp,compare);
        savecomp.close();
        if(output.substr(pubdate+8,31) == compare)
        {
            //they are the same. no update
            //cout << "No update" << endl;
        }
        else
        {
            //update savedate file with the new update
            ofstream saveout("savedate.txt");
            saveout << output.substr(pubdate+8,31) << endl;
            saveout.close();

            //output screen saying update
            //cout << "UPDATE!!!" << endl;

            //Setting up the image
            sf::Image upd_img;
            sf::Texture upd_tex;
            upd_img.loadFromFile("update.gif");
            upd_tex.loadFromFile("update.gif");
            sf::Sprite upd_spr;
            upd_spr.setTexture(upd_tex);
            //upd_spr.setScale(.5,.5);
            sf::Vector2u upd_size =  upd_img.getSize();


            // Create the main window
            sf::RenderWindow App(sf::VideoMode(upd_size.x, upd_size.y), "MSPA Update",   sf::Style::None);
            sf::VideoMode screens =  sf::VideoMode::getDesktopMode();
            App.setPosition(sf::Vector2i(screens.width-upd_size.x, 35));

            while (App.isOpen())
            {
                // Event processing
                sf::Event event;
                while (App.pollEvent(event))
                {
                    // Request for closing the window
                    if (event.type == sf::Event::Closed)
                        App.close();

                    //pressing escape
                    if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
                        App.close();
                }

                // Activate the window for OpenGL rendering
                App.setActive();

                // OpenGL drawing commands go here...
                App.draw(upd_spr);

                // End the current frame and display its contents on screen
                App.display();
            }
        }

        //sleep for total_sleep minutes
        Sleep(total_sleep);



        //always cleanup
        curl_easy_cleanup(curl);
    }
    fileout.close();
    cout << "done" << endl;
    }
    return 0;
}
