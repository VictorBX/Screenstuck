#include "functions.h"

int main(void)
{
    unsigned int total_wait = intro();

    while(true)
    {
        //curl initialize
        CURL *curl;
        CURLcode res;

        //location at which xml is saved
        ofstream fileout("xmldata.txt");

        //location of the publication date and link on xml
        int pubdate = 0;
        int linkloc = 0;
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

            //grabbing the update time
            string output = stream.str();
            fileout << output << std::endl;
            loc = output.find("pubDate");
            pubdate = int(loc);
            int start_pdate = pubdate+8;
            int length_pdate = 31;
            string save = output.substr(start_pdate,length_pdate);

            //grabbing the new comic link
            loc = output.find("p=");
            linkloc = int(loc);
            int start_link = linkloc+2;
            int length_link = 6;
            string link_num = output.substr(start_link,length_link);

            //if the saveout file is empty, then write the date of the
            //newest update and comic link in the saveout file
            if(file_empty()==true)
            {
                ofstream saveout("savedate.txt");
                saveout << output.substr(start_pdate,length_pdate) << endl;
                saveout << link_num << endl;
                saveout.close();
            }

            //Using the new xml file in fileout, check the new date of
            //the latest update and compare it to the date in saveout
            //every minute. If different, there is an update.
            //Also grab the old comic link
            ifstream savecomp("savedate.txt");
            string compare;
            string link_comp;
            getline(savecomp,compare);
            getline(savecomp,link_comp);
            savecomp.close();
            if(output.substr(start_pdate,length_pdate) != compare)
            {
                //update savedate file with the new update and comic link
                ofstream saveout("savedate.txt");
                saveout << output.substr(start_pdate,length_pdate) << endl;
                saveout << output.substr(start_link, length_link) << endl;
                saveout.close();

                //updating comic link by 1 to start the comic from the first page update
                int link_inum = atoi(link_comp.c_str()) + 1;
                stringstream itos;
                itos << link_inum;
                string new_page = itos.str();

                //output screen saying update
                //Setting up the image
                sf::Image upd_img;
                sf::Texture upd_tex;

                if(!upd_img.loadFromFile("update.gif"));
                if(!upd_tex.loadFromFile("update.gif"));

                sf::Sprite upd_spr;
                upd_spr.setTexture(upd_tex);
                sf::Vector2u upd_size =  upd_img.getSize();


                // Create the main window
                sf::RenderWindow App(sf::VideoMode(upd_size.x, upd_size.y), "MSPA Update",   sf::Style::None);
                sf::VideoMode screens =  sf::VideoMode::getDesktopMode();
                App.setPosition(sf::Vector2i(screens.width-upd_size.x, 0.0));

                while (App.isOpen())
                {
                    // Event processing
                    sf::Event event;
                    while (App.pollEvent(event))
                    {
                        //request for closing the window
                        if (event.type == sf::Event::Closed)
                            App.close();

                        //pressing escape
                        if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
                        {
                            App.close();
                            //pop-up or open a new tab with the first new page of the comic
                            string url_input = "http://www.mspaintadventures.com/?s=6&p=00"+new_page;
                            ShellExecute(NULL, "open", url_input.c_str(), NULL, NULL, SW_SHOWNORMAL);
                        }

                        //clicking the image to close window
                        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        {
                            sf::Vector2i mouse_pos = sf::Mouse::getPosition();
                            if(mouse_pos.x >screens.width-upd_size.x && mouse_pos.y < upd_size.y)
                            {
                                App.close();
                                //pop-up or open a new tab with the first new page of the comic
                                string url_input = "http://www.mspaintadventures.com/?s=6&p=00"+new_page;
                                ShellExecute(NULL, "open", url_input.c_str(), NULL, NULL, SW_SHOWNORMAL);
                            }
                        }
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
            wait_time(total_wait);

            //always cleanup
            curl_easy_cleanup(curl);
        }

        fileout.close();
    }

    return 0;
}
