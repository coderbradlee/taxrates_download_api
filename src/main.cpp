
#include "serverResource.hpp"
#include "paypal.hpp"


int main() {
	try
	{
			boost::timer::cpu_timer pass;
			pass.start();
			std::vector<std::string> states = { "AK","AL","AR","AZ","CA","CO","CT","DE","FL","GA","HI","IA","ID","IL","IN","KS","KY","LA","MA","MD","ME","MI","MN","MO","MS","MT","NC","ND","NE","NH","NJ","NM","NV","NY","OH","OK","OR","PA","RI","SC","SD","TN","TX","UT","VT","VA","WA","WI","WV","WY","DC","PR"};
			ptime now = second_clock::local_time();
			string year_mon = to_iso_string(now.date()).substr(0, 6);
			string now_str = year_mon + ".csv";
		
			boost::thread_group tg;
			//std::string save_path = uploads_init_dir() + "/" + year_mon;
			std::string save_path = uploads_init_dir();
			boost::this_thread::sleep_for(boost::chrono::seconds(3));
			boost::filesystem::create_directories(save_path);
			
			//cout << __LINE__ << ":" << save_path << endl;
			for (auto& state:states)
			{
				//cout<<++i<<endl;
				std::string filename = "TAXRATES_ZIP5_" + state + now_str ;
				//cout << filename << endl;
				tg.create_thread(bind(thread_download, filename, save_path));

			}
			tg.join_all();
			
			std::cout << "now time elapsed:" << pass.format(6) << std::endl;

			boost::this_thread::sleep_for(boost::chrono::seconds(15));


		return EXIT_SUCCESS;

	}
	catch (std::exception& e) {
		//cout << diagnostic_information(e) << endl;
		cout << e.what() << endl;
        }
	catch(...) 
	{
         
	}
    return 0;
}