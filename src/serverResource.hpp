#ifndef SERVER_RESOURCE_HPP
#define	SERVER_RESOURCE_HPP
#define BOOST_SPIRIT_THREADSAFE
#include <boost/regex.hpp>
#include "renesolalog.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <boost/bind.hpp>
#include <list>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <condition_variable>
#include <assert.h>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>

#include <boost/thread/mutex.hpp>


//Added for the default_resource example
#include<fstream>
using namespace std;
//Added for the json:
using namespace boost::property_tree;
using namespace boost::posix_time;

#include <restbed>
#include "client_http.hpp"
//#include "server_http.hpp"
#include "curl_client.hpp"
#include "csv.h"
using namespace restbed;
//typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;
//////////////default
void default_handler(const std::shared_ptr< Session > session)
{
	try
	{
		cout << "default handler" << endl;
		/*string body = "this is default page";
		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });	*/
		const auto request = session->get_request();
		string filename = request->get_path_parameter("filename");
		cout << filename << endl;
		if (filename.length() == 0)
		{
			filename = "index.html";
		}
		ifstream stream("./resource/" + filename, ifstream::in);

		if (stream.is_open())
		{
			const string body = string(istreambuf_iterator< char >(stream), istreambuf_iterator< char >());

			const multimap< string, string > headers
			{
				{ "Content-Type", "text/html" },
				{ "Content-Length", ::to_string(body.length()) }
			};

			session->close(OK, body, headers);
		}
		else
		{
			session->close(NOT_FOUND);
		}
	}
	catch (std::exception& ex)
	{
		cout << ex.what() << endl;
		session->close(NOT_FOUND);
	}
}
/////////////error
void faulty_method_handler(const std::shared_ptr< Session >)
{
	throw SERVICE_UNAVAILABLE;
}

void resource_error_handler(const int, const std::exception&, const std::shared_ptr< Session > session)
{
	if (session->is_open())
	{
		session->close(6000, "Custom Resource Internal Server Error", { { "Content-Length", "37" } });
	}
	else
	{
		session->close();
		fprintf(stderr, "Custom Resource Internal Server Error\n");
	}
}

void service_error_handler(const int, const std::exception& ex, const std::shared_ptr< Session > session)
{
	if (session->is_open())
	{
		cout << ex.what() << endl;
		session->close(5000, "Custom Service Internal Server Error", { { "Content-Length", "36" } });
	}
	else
	{
		session->close();
		fprintf(stderr, "Custom Service Internal Server Error\n");
	}
}

////////////////////auth
void authentication_handler(const std::shared_ptr< Session > session,
	const function< void(const std::shared_ptr< Session >) >& callback)
{
	const auto request = session->get_request();
	auto authorization = request->get_header("Authorization");
	// curl -w'\n' -v -XGET 'http://testapi@orderbot.com:ShinyElephant232#@localhost:8688/resource'
	// curl -u testapi@orderbot.com:ShinyElephant232# -X GET http://localhost:8688/resource
	//cout << authorization << endl;

	/*BOOST_LOG_SEV(slg, boost_log->get_log_level()) << authorization;
	boost_log->get_initsink()->flush();*/



	cout << request->get_path() << endl;
	{
		/*string body = "";
		for (const auto key : session->keys())
		{
			string value = session->get(key);
			body += key + ":" + value + "\n";

		}
		cout << body << endl;

		auto ret = request->get_query_parameters();
		for (auto& r : ret)
			cout << r.first << ":" << r.second << endl;*/

		/*size_t content_length = 0;
		request->get_header("Content-Length", content_length);
		cout << "Content-Length:" << content_length << endl;*/
		//cout << "url:" << request->get_header("url").get_query_parameters() << endl;


		//session->fetch(content_length, [request](const std::shared_ptr< Session > session, const Bytes & body)
		//{
		//	fprintf(stdout, ":%.*s\n", (int)body.size(), body.data());
		//	//session->close(OK, "Hello, World!", { { "Content-Length", "13" }, { "Connection", "close" } });
		//});
	}

	if (authorization not_eq "Basic dGVzdGFwaUBvcmRlcmJvdC5jb206U2hpbnlFbGVwaGFudDIzMiM=")
	{
		session->close(UNAUTHORIZED, { { "WWW-Authenticate", "Basic realm=\"restbed\"" } });
	}
	else
	{
		callback(session);
	}
}
/////////////////regular
void get_orders_num_func(const std::shared_ptr< Session > session)
{/*
	stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/
	string first = "{ \"order_id\":";
	string last = ", \"customer_po\" : null, \"order_date\" : \"2015-04-16\", \"last_modified\" : \"2015-08-13\", \"ship_date\" : \"2015-04-16\", \"shipping_method\" : \"UPS Ground\", \"order_status\" : \"unshipped\", \"customer_id\" : 1, \"order_tax_total\" : 0, \"shipping_total\" : 0, \"discount_total\" : 0, \"order_total\" : 0, \"notes\" : \"\", \"internal_notes\" : \"\", \"shipping_address\" : {			\"store_name\": null, \"full_name\" : \"John Smith\", \"street1\" : \"123 1st St.\", \"street2\" : \"\", \"city\" : \"San Francisco\", \"state\" : \"CA\", \"postal_code\" : \"11223\", \"country\" : \"US\", \"phone\" : \"5555555555\", \"fax\" : null, \"email\" : \"support@orderbot.com\", Orderbot API				November 23rd, 2015 12				\"website\" : null		}, \"billing_address\" : {				\"sales_channel\": \"DTC\", \"full_name\" : \"John Smith\", \"street1\" : \"123 1st St.\", \"street2\" : \"\", \"city\" : \"San Francisco\", \"state\" : \"CA\", \"postal_code\" : \"11223\", \"country\" : \"US\", \"phone\" : \"5555555555\", \"fax\" : null, \"email\" : \"support@orderbot.com\", \"website\" : null			}, \"payment\" : [{					\"payment_method\": \"Paid From Web\", \"amount_paid\" : 0.1				}, { \"payment_method\": \"VOID\", \"amount_paid\" : -24.96 }, { \"payment_method\": \"Credit\", \"amount_paid\" : 24.96 }, { \"payment_method\": \"Customer Service\", \"amount_paid\" : -0.1 }], \"items\" : [{				\"item_id\": 0, \"product_id\" : 96211, \"sku\" : \"ASDF123\", \"name\" : \"Test Product\", \"quantity\" : 1, \"unit_price\" : 0, \"discount\" : 0, \"product_tax\" : 0, \"product_tax_total\" : 0, \"product_total\" : 0, \"weight\" : 0.5			}], \"tracking_numbers\" : null, \"other_charges\" : null }";
	string order_num = session->get_request()->get_path_parameter("name");
	string body = first + order_num + last;
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void put_orders_num_func(const std::shared_ptr< Session > session)
{
	const auto request = session->get_request();

	/*auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << "::::" << r.second << endl;*/
	string cbody = "";
	for (const auto key : session->keys())
	{
		string value = session->get(key);
		cbody += key + ":::::" + value + "\n";
		cout << key <<":::::"<< value<< endl;
	}
	cout << __LINE__ << ":" << cbody << endl;

	size_t content_length = 0;
	request->get_header("Content-Length", content_length);

	session->fetch(content_length, [request](const std::shared_ptr< Session > session, const Bytes & body)
	{
		fprintf(stdout, "%.*s\n", (int)body.size(), body.data());
		//session->close(OK, "Hello, World!", { { "Content-Length", "13" }, { "Connection", "close" } });
	});
	string order_num = session->get_request()->get_path_parameter("name");
	cout << "order_num:" << order_num << endl;
	string body = "{ \"response_code\": 1, \"orderbot_order_id\" : 2, \"reference_order_id\" : null, \"success\" : true, \"message\" : \"Order has been updated successfully!\" }";
	
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void post_orders_param_func(const std::shared_ptr< Session > session)
{
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		//fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());
		//session->close(OK, "Hello, World!", { { "Content-Length", "13" }, { "Connection", "close" } });

		string body = "{\"response_code\": 1,\"message\": \"Success\",\"order_process_result\": [{\"response_code\": 0,\"orderbot_order_id\": 79,\"reference_order_id\": \"aabb15998966\",\"success\": true,\"message\": \"Order has been placed successfully!\"}]}";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
		
}
void get_orders_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[{\"order_id\": 2, \"customer_po\" : null, \"order_date\" : \"2015-04-16\", \"last_modified\" : \"2015-08-13\", \"ship_date\" : \"2015-04-16\", \"shipping_method\" : \"UPS Ground\", \"order_status\" : \"unshipped\", \"customer_id\" : 1, \"order_tax_total\" : 0, \"shipping_total\" : 0, \"discount_total\" : 0, \"order_total\" : 0, \"notes\" : \"\", \"internal_notes\" : \"\", \"shipping_address\" : {\"store_name\": null, \"full_name\" : \"John Smith\", \"street1\" : \"123 1st St.\", \"street2\" : \"\", \"city\" : \"San Francisco\", \"state\" : \"CA\", \"postal_code\" : \"11223\", \"country\" : \"US\", \"phone\" : \"5555555555\", \"fax\" : null, \"email\" : \"support@orderbot.com\", \"website\" : null}, \"billing_address\" : {\"sales_channel\": \"DTC\", \"full_name\" : \"John Smith\", \"street1\" : \"123 1st St.\", \"street2\" : \"\", \"city\" : \"San Francisco\", \"state\" : \"CA\", \"postal_code\" : \"11223\", \"country\" : \"US\", \"phone\" : \"5555555555\", \"fax\" : null, \"email\" : \"support@orderbot.com\", \"website\" : null}, \"payment\" : [{\"payment_method\": \"Paid From Web\", \"amount_paid\" : 0.1}, { \"payment_method\": \"VOID\", \"amount_paid\" : -24.96 }, { \"payment_method\": \"Credit\", \"amount_paid\" : 24.96 }, { \"payment_method\": \"Customer Service\", \"amount_paid\" : -0.1 }], \"items\" : [{\"item_id\": 0, \"product_id\" : 96211, \"sku\" : \"ASDF123\", \"name\" : \"Test Product\", \"quantity\" : 1, \"unit_price\" : 0, \"discount\" : 0, \"product_tax\" : 0, \"product_tax_total\" : 0, \"product_total\" : 0, \"weight\" : 0.5}], \"tracking_numbers\" : null, \"other_charges\" : null}, { \"order_id\": 4, \"customer_po\" : \"MONCLAIR 430\", \"order_date\" : \"2015-05-01\", \"last_modified\" : \"2015-05-20\", \"ship_date\" : \"2015-05-01\", \"shipping_method\" : \"Express\", \"order_status\" : \"unshipped\", \"customer_id\" : 1, \"order_tax_total\" : 50, \"shipping_total\" : 50, \"discount_total\" : 0, \"order_total\" : 2373, \"notes\" : \"\", \"internal_notes\" : \"Please quote white glove ASDF123\", \"shipping_address\" : {\"store_name\": null, \"full_name\" : \"test contact name \", \"street1\" : \"123 Will Let You Know\", \"street2\" : \"\", \"city\" : \"New Hyde Park\", \"state\" : \"NY\", \"postal_code\" : \"11040\", \"country\" : \"US\", \"phone\" : \"7788721267\", \"fax\" : null, \"email\" : \"support@orderbot.com\", \"website\" : null}, \"billing_address\" : {\"sales_channel\": \"DTC\", \"full_name\" : \" \", \"street1\" : \"347 West 36th St\", \"street2\" : \"Suite 1300\", \"city\" : \"New York\", \"state\" : \"NY\", \"postal_code\" : \"10018\", \"country\" : \"US\", \"phone\" : \"\", \"fax\" : null, \"email\" : \"support@orderbot.com\", \"website\" : null}, \"payment\" : [], \"items\" : [{\"item_id\": 0, \"product_id\" : 96211, \"sku\" : \"ASDF123\", \"name\" : \"Test Product\", \"quantity\" : 1, \"unit_price\" : 2273, \"discount\" : 0, \"product_tax\" : 0, \"product_tax_total\" : 0, \"product_total\" : 2273, \"weight\" : 0.5}], \"tracking_numbers\" : null, \"other_charges\" : null }]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}

void get_products_num_func(const std::shared_ptr< Session > session)
{/*
	stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/
	string first = "{ \"product_category_id\": 2, \"product_category\": \"Rings\", \"product_group_id\": 4, \"product_group\": \"Bands\", \"product_id\": 469315, \"product_name\": \"TestAPI \", \"sku\": \"testAPI\", \"upc\": \"12345678\", \"cost\": 2, \"descriptive_title\": \"\", \"description\": \"\", \"is_parent\": false,\"parent_id\": 0, \"units_of_measure\": \"Each\", \"taxable\": true, \"shipping_units_of_measure\": \"Lbs\", \"weight\": 1, \"active\": true, \"base_price\": 10, \"lead_times\": null, \"variable_group_id\": 0, \"variable_group_name\": null, \"variable1_id\": 0, \"variable1_name\": null, \"variable_Value1_id\": 0, \"variable_value1_name\": null, \"variable2_id\": 0, \"variable2_name\": null, \"variable_value2_id\": 0, \"variable_value2_name\": null, \"custom_fields\": [], \"tags\": [], \"image_urls\": [], \"visibility_on_webs\": null, \"optional_description_fields\": { \"description1\": \"\", \"description2\": \"\", \"description3\": \"\", \"description4\": \"\", \"description5\": \"\", \"description6\": \"\" } }";
	string order_num = session->get_request()->get_path_parameter("name");
	string body = first;
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void put_products_num_func(const std::shared_ptr< Session > session)
{
	string product_num = session->get_request()->get_path_parameter("name");
	cout << "product_num:" << product_num << endl;
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());
		
		string body = "{  \"success\": true,  \"message\": \"Prouct update success\",  \"reference_product_id\": null,  \"orderbot_product_id\": 429229  }";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});


}
void get_products_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[{\"product_category_id\": 2, \"product_category\" : \"Rings\", \"product_group_id\" : 4, \"product_group\" : \"Bands\", \"product_id\" : 469315, \"product_name\" : \"TestAPI \", \"sku\" : \"testAPI\", \"upc\" : \"12345678\", \"cost\" : 2, \"descriptive_title\" : \"\", \"description\" : \"\", \"is_parent\" : false, \"parent_id\" : 0, \"units_of_measure\" : \"Each\", \"taxable\" : true, \"shipping_units_of_measure\" : \"Lbs\", \"weight\" : 1, \"active\" : true, \"base_price\" : 10, \"lead_times\" : null, \"variable_group_id\" : 0, \"variable_group_name\" : null, \"variable1_id\" : 0, \"variable1_name\" : null, \"variable_Value1_id\" : 0, \"variable_value1_name\" : null, \"variable2_id\" : 0, \"variable2_name\" : null, \"variable_value2_id\" : 0, \"variable_value2_name\" : null, \"custom_fields\" : [], \"tags\" : [], \"image_urls\" : [], \"visibility_on_web\" : null, \"optional_description_fields\" : {\"description1\": \"\", \"description2\" : \"\", \"description3\" : \"\", \"description4\" : \"\", \"description5\" : \"\", \"description6\" : \"\"}}, { \"product_category_id\": 2, \"product_category\" : \"Rings\", \"product_group_id\" : 3, \"product_group\" : \"With Stones\", \"product_id\" : 469316, \"product_name\" : \"Ring Stone\", \"sku\" : \"stone\", \"upc\" : \"\", \"cost\" : 3, \"descriptive_title\" : \"\", \"description\" : \"\", \"is_parent\" : false, \"parent_id\" : 0, \"units_of_measure\" : \"Each\", \"taxable\" : false, \"shipping_units_of_measure\" : \"Lbs\", \"weight\" : 1, \"active\" : true, \"base_price\" : 12, \"lead_times\" : null, \"variable_group_id\" : 0, \"variable_group_name\" : null, \"variable1_id\" : 0, \"variable1_name\" : null, \"variable_Value1_id\" : 0, \"variable_value1_name\" : null, \"variable2_id\" : 0, \"variable2_name\" : null, \"variable_value2_id\" : 0, \"variable_value2_name\" : null, \"custom_fields\" : [], \"tags\" : [], \"image_urls\" : [], \"visibility_on_webs\" : null, \"optional_description_fields\" : {\"description1\": \"test1\", \"description2\" : \"test2 \", \"description3\" : \"\", \"description4\" : \"\", \"description5\" : \"\", \"description6\" : \"\"} }]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void post_products_param_func(const std::shared_ptr< Session > session)
{
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());
		
		string body = "[  {  \"success\": true,  \"message\": \"Product created success; BOM created success; Purchase unit created success\",  \"reference_product_id\": \"1000\",  \"orderbot_product_id\": 469317  },  {  \"success\": true,  \"message\": \"Product created success; BOM created success; Purchase unit created success\",  \"reference_product_id\": \"1001\",  \"orderbot_product_id\": 469318  }  ]";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}

void get_customers_num_func(const std::shared_ptr< Session > session)
{/*
	stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/
	string first = "{  \"customer_id\": 2,  \"account_id\": 2,  \"customer_name\": \" \",  \"address\": \"555\",  \"address2\": \"\",  \"city\": \"Vancouver\",  \"state\": \"AL\",  \"country\": \"US\",  \"postal_code\": \"90210\",  \"phone\": \"5555555555\",  \"phone_ext\": \"\",  \"sales_rep_id\": null,  \"commission_rate\": 0,  \"email\": \"\",  \"customer_hash_id\": null,  \"other_id\": \"7777777\",  \"active\": true  }";
	string order_num = session->get_request()->get_path_parameter("name");
	string body = first;
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void put_customers_num_func(const std::shared_ptr< Session > session)
{
	
	string product_num = session->get_request()->get_path_parameter("name");
	cout << "product_num:" << product_num << endl;
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "{  \"reference_customer_id\": \"7777777\",  \"orderbot_customer_id\": 2,  \"success\": true,  \"message\": \"Customer updated successfully\"  }";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});

}
void get_customers_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[{\"customer_id\": 2,\"account_id\": 2,\"customer_name\": \" \",\"address\": \"555\",\"address2\": \"\",\"city\": \"Vancouver\",\"state\": \"AL\",\"country\": \"US\",\"postal_code\": \"90210\",\"phone\": \"5555555555\",\"phone_ext\": \"\",\"sales_rep_id\": null,\"commission_rate\": 0,\"email\": \"\",\"customer_hash_id\": null,\"other_id\": \"7777777\",\"active\": true},{\"customer_id\": 3,\"account_id\": 3,\"customer_name\": \" \",\"address\": \"555\",\"address2\": \"\",\"city\": \"Vancouver\",\"state\": \"AL\",\"country\": \"US\",\"postal_code\": \"90210\",\"phone\": \"5555555555\",\"phone_ext\": \"\",\"sales_rep_id\": null,\"commission_rate\": 0,\"email\": \"\",\"customer_hash_id\": null,\"other_id\": null,\"active\": true},{\"customer_id\": 9,\"account_id\": 9,\"customer_name\": \"Peter Lin\",\"address\": \"122 sdgdfg\",\"address2\": \"12342\",\"city\": \"sfsdf\",\"state\": \"AL\",\"country\": \"US\",\"postal_code\": \"98001\",\"phone\": \"11111111\",\"phone_ext\": \"\",\"sales_rep_id\": 461,\"commission_rate\": 0,\"email\": \"Test@orderbot.com\",\"customer_hash_id\": null,\"other_id\": null,\"active\": true}]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void post_customers_param_func(const std::shared_ptr< Session > session)
{
	
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "[  {  \"reference_account_id\": \"1000111\",  \"orderbot_account_id\": 36,  \"customers\": [  {  \"reference_customer_id\": \"10001\",  \"orderbot_customer_id\": 36,  \"success\": true,  \"message\": \"Customer successfully created\"  }  ],  \"success\": true,  \"message\": \"Account successfully created\"  },  {  \"reference_account_id\": \"1000113\",  \"orderbot_account_id\": 37,  \"customers\": [  {  \"reference_customer_id\": \"10001\",  \"orderbot_customer_id\": 37,  \"success\": true,  \"message\": \"Customer successfully created\"  }  ],  \"success\": true,  \"message\": \"Account successfully created\"  }  ]";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}



void get_salesreps_num_func(const std::shared_ptr< Session > session)
{/*
	stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/
	string first = "{\"sales_rep_id\": 461,\"sales_name\":\"Andrew\",\"commission\": 8,\"email_invoices\":null}";
	string order_num = session->get_request()->get_path_parameter("name");
	string body = first;
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void put_salesreps_num_func(const std::shared_ptr< Session > session)
{ 
	string salesreps_num = session->get_request()->get_path_parameter("name");
	cout << "salesreps_num:" << salesreps_num << endl;
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "{  \"success\": true,  \"sales_rep_id\": 461,  \"sales_name\": \"Peter\"  }";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}
void get_salesreps_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	/*auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;*/
	string body = "[{\"sales_rep_id\": 461,\"sales_name\": \"Andrew\",\"commission\": 8,\"email_invoices\": null},{\"sales_rep_id\": 463,\"sales_name\": \"test s\",\"commission\": 0,\"email_invoices\": null},{\"sales_rep_id\": 509,\"sales_name\": \"Sales User\",\"commission\": 0,\"email_invoices\": null},{\"sales_rep_id\": 557,\"sales_name\": \"Peter\",\"commission\": 10,\"email_invoices\": null},{\"sales_rep_id\": 558,\"sales_name\": \"Sales Rep\",\"commission\": 5,\"email_invoices\": \"\"},{\"sales_rep_id\": 559,\"sales_name\": \"Anna\",\"commission\": 6,\"email_invoices\": null}]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void post_salesreps_param_func(const std::shared_ptr< Session > session)
{
	
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "[  {  \"success\": true,  \"sales_rep_id\": 560,  \"sales_name\": \"Peter\"  }  ]";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}

void put_product_categories_num_func(const std::shared_ptr< Session > session)
{ 
	string product_categories_num = session->get_request()->get_path_parameter("name");
	cout << "product_categories_num:" << product_categories_num << endl;
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "{  \"success\": true,  \"is_valid\": true,  \"ref_category_id\": \"123\",  \"product_category_id\": 55,  \"product_category_name\": \"test\",  \"message\": \"Updated success\"  }";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}
void post_product_categories_param_func(const std::shared_ptr< Session > session)
{
	
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "[  {  \"success\": true,  \"is_valid\": true,  \"ref_category_id\": \"123\",  \"product_category_id\": 55,  \"product_category_name\": \"test\",  \"message\": \"Uploaded success\"  }  ]";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}

void put_product_groups_num_func(const std::shared_ptr< Session > session)
{ 
	string put_product_groups_num = session->get_request()->get_path_parameter("name");
	cout << "put_product_groups_num:" << put_product_groups_num << endl;
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "{  \"success\": true,  \"is_valid\": true,  \"ref_group_id\": \"123\",  \"product_group_id\": 55,  \"product_group_name\": \"test\",  \"message\": \"Updated success\"  }";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}
void post_product_groups_param_func(const std::shared_ptr< Session > session)
{
	
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "[  {  \"success\": true,  \"is_valid\": true,  \"ref_group_id\": \"123\",\"product_group_id\": 55,  \"product_group_name\": \"test\",  \"message\": \"Uploaded success\"  }  ]";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}
void put_variable_groups_num_func(const std::shared_ptr< Session > session)
{ 
	string put_variable_groups_num_func = session->get_request()->get_path_parameter("name");
	cout << "put_variable_groups_num_func:" << put_variable_groups_num_func << endl;
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "{  \"success\": true,  \"is_valid\": true,  \"variable_group_id\": 55,  \"variable_group_name\": \"test\",  \"message\": \"Updated success\"  }";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}


void post_product_variable_groups_param_func(const std::shared_ptr< Session > session)
{
	
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "[  {  \"success\": true,  \"is_valid\": true,  \"variable_group_id\": 55,  \"variable_group_name\": \"test\",  \"message\": \"Uploaded success\"  }  ]";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}
void put_product_variables_num_func(const std::shared_ptr< Session > session)
{ 
	string put_product_variables_num = session->get_request()->get_path_parameter("name");
	cout << "put_product_variables_num:" << put_product_variables_num << endl;
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "{  \"success\": true,  \"is_valid\": true,  \"variable_group_id\": 55,  \"variable_name\": \"test\",  \"message\": \"Updated success\"  }";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}

void post_product_variables_param_func(const std::shared_ptr< Session > session)
{
	
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "[  {  \"success\": true,  \"is_valid\": true,  \"variable_group_id\": 55,  \"variable_name\": \"test\",  \"message\": \"Uploaded success\"  }  ]";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}
void put_product_variable_values_num_func(const std::shared_ptr< Session > session)
{ 
	string put_product_variable_values_num_func = session->get_request()->get_path_parameter("name");
	cout << "put_product_variable_values_num_func:" << put_product_variable_values_num_func << endl;
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "{  \"success\": true,  \"is_valid\": true,  \"variable_id\": 55,  \"variable_value_name\": \"test\",  \"message\": \"Updated success\"  }";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}

void post_product_variable_values_param_func(const std::shared_ptr< Session > session)
{
	
	const auto request = session->get_request();
	size_t content_length = 0;
	request->get_header("Content-Length", content_length);
	cout << content_length << endl;
	//string ret = session->get_body();
	session->fetch(content_length, [&](const std::shared_ptr< Session > session, const Bytes & content_body)
	{
		fprintf(stdout, "%.*s\n", (int)content_body.size(), content_body.data());

		string body = "[  {  \"success\": true,  \"is_valid\": true,  \"variable_id\": 55,  \"variable_value_name\": \"test\",  \"message\": \"Uploaded success\"  }  ]";

		session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
	});
}



void get_account_groups_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"account_groups\" : [ { \"account_group_id\" : 18,\"account_group_name\" : \"Default\"},{ \"account_group_id\" : 33,\"account_group_name\" : \"Default\"},{ \"account_group_id\" : 34,\"account_group_name\" : \"Default\"},{ \"account_group_id\" : 91,\"account_group_name\" : \"Default\"},{ \"account_group_id\" : 92,\"account_group_name\" : \"Default\"}],\"sales_channel_id\" : 1,\"sales_channel_name\" : \"DTC\"}]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void get_units_of_measurement_types_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"name\" : \"Each\",\"units_of_measure_id\" : 1},{ \"name\" : \"Pk\",\"units_of_measure_id\" : 2},{ \"name\" : \"Lbs\",\"units_of_measure_id\" : 4},{ \"name\" : \"v\",\"units_of_measure_id\" : 8}]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void get_order_guides_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"name\" : \"Default\",\"order_guide_id\" : 364,\"sales_channel_id\" : 1,\"sales_channel_name\" : \"DTC\"},{ \"name\" : \"Default\",\"order_guide_id\" : 511,\"sales_channel_id\" : 2,\"sales_channel_name\" : \"Wholesale\"},{ \"name\" : \"Default\",\"order_guide_id\" : 512,\"sales_channel_id\" : 4,\"sales_channel_name\" : \"Distributors\"}]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void get_product_structure_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"class_type_id\" : 2,\"class_type_name\" : \"Assembly\",\"product_classes\" : [ { \"categories\" : [ ],\"product_class_id\" : 26,\"product_class_name\" : \"Assembly\"},{ \"categories\" : [ ],\"product_class_id\" : 312,\"product_class_name\" : \"Assembly2\"},{ \"categories\" : [ ],\"product_class_id\" : 317,\"product_class_name\" : \"Assembly3\"}]},{ \"class_type_id\" : 1,\"class_type_name\" : \"Component\",\"product_classes\" : [ { \"categories\" : [ { \"category_id\" : 616,\"category_name\" : \"Your category1 \",\"groups\" : [ { \"group_id\" : 2631,\"group_name\" : \"Your group1 \"},{ \"group_id\" : 2632,\"group_name\" : \" Your group2\"}]} ],\"product_class_id\" : 25,\"product_class_name\" : \"Inventory 1\"},{ \"categories\" : [ ],\"product_class_id\" : 83,\"product_class_name\" : \"Inventory 2\"},{ \"categories\" : [ ],\"product_class_id\" : 311,\"product_class_name\" : \"Inventory4\"},{ \"categories\" : [ ],\"product_class_id\" : 315,\"product_class_name\" : \"Inventory5\"}]},{ \"class_type_id\" : 4,\"class_type_name\" : \"Sales\",\"product_classes\" : [ { \"categories\" : [ { \"category_id\" : 1,\"category_name\" : \"Your category1\",\"groups\" : [ { \"group_id\" : 1,\"group_name\" : \"Your group1\"},{ \"group_id\" : 2,\"group_name\" : \"Your group2\"}]},{ \"category_id\" : 2,\"category_name\" : \"Rings\",\"groups\" : [ { \"group_id\" : 3,\"group_name\" : \"With Stones\"},{ \"group_id\" : 4,\"group_name\" : \"Bands\"}]}],\"product_class_id\" : 33,\"product_class_name\" : \"Products\"} ]}]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void get_product_variables_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"product_variables\" : [ { \"product_variable_values\" : [ { \"variable_value_id\" : 317,\"variable_value_name\" : \"XS\"},{ \"variable_value_id\" : 318,\"variable_value_name\" : \"S\"},{ \"variable_value_id\" : 319,\"variable_value_name\" : \"M\"},{ \"variable_value_id\" : 320,\"variable_value_name\" : \"L\"},{ \"variable_value_id\" : 321,\"variable_value_name\" : \"XL\"}],\"variable_id\" : 37,\"variable_name\" : \"Sizes\"},{ \"product_variable_values\" : [ { \"variable_value_id\" : 322,\"variable_value_name\" : \"Black\"},{ \"variable_value_id\" : 323,\"variable_value_name\" : \"Blue\"},{ \"variable_value_id\" : 324,\"variable_value_name\" : \"Green\"},{ \"variable_value_id\" : 325,\"variable_value_name\" : \"Red\"},{ \"variable_value_id\" : 326,\"variable_value_name\" : \"Orange\"},{ \"variable_value_id\" : 327,\"variable_value_name\" : \"Pink\"},{ \"variable_value_id\" : 328,\"variable_value_name\" : \"Purple\"},{ \"variable_value_id\" : 329,\"variable_value_name\" : \"White\"},{ \"variable_value_id\" : 330,\"variable_value_name\" : \"Yellow\"}],\"variable_id\" : 38,\"variable_name\" : \"Colours\"}],\"variable_group_id\" : 37,\"variable_group_name\" : \"General\"} ]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}

void get_purchase_unit_categories_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"purchase_unit_category_id\" : 1,  \"purchase_unit_category_name\" : \"Default\"  },  { \"purchase_unit_category_id\" : 218,  \"purchase_unit_category_name\" : \"\"  }  ]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void get_vendor_purchase_unit_of_measurements_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"vendor_purchase_unit_of_measurement_id\" : 61,  \"vendor_purchase_unit_of_measurement_name\" : \"Each\"  } ]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void get_salesrep_groups_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"commission\" : 0.0,\"sales_rep_group_id\" : 1,\"sales_rep_group_name\" : \"Default\"} ]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void get_Vendors_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"vendor_id\" : 864,  \"vendor_name\" : \"TEST VENDOR\"  },  { \"vendor_id\" : 1840,  \"vendor_name\" : \"USD Test Vendor\"  }  ]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void get_Countries_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"country\" : \"Canada\",\"country_id\" : 38,\"iso2\" : \"CA\",\"iso3\" : \"CAN\"},{ \"country\" : \"United States\",\"country_id\" : 226,\"iso2\" : \"US\",\"iso3\" : \"USA\"}]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void get_States_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"abbreviation\" : \"AL\",  \"state\" : \"Alabama\",  \"state_id\" : 1  },  { \"abbreviation\" : \"AK\",  \"state\" : \"Alaska\",  \"state_id\" : 2  },  { \"abbreviation\" : \"AZ\",  \"state\" : \"Arizona\",  \"state_id\" : 3  },  { \"abbreviation\" : \"AR\",  \"state\" : \"Arkansas\",  \"state_id\" : 4  }]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void get_distribution_centers_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"address\" : \"Warehouse address\",\"address2\" : \"\",\"city\" : \"Warehouse city\",\"contact\" : \"warehosue contact\",\"country\" : \"US\",\"distribution_center_id\" : 1,\"distribution_center_name\" : \"Your warehouse\",\"email\" : \"warehouse@email.com\",\"fax\" : \"\",\"phone\" : \"999 - 888 - 7777\",\"postal_code\" : \"11111\",\"state\" : \"AL\",\"website\" : \"\"},{ \"address\" : \"New address \",\"address2\" : \"\",\"city\" : \"New City\",\"contact\" : \"\",\"country\" : \"CA\",\"distribution_center_id\" : 138,\"distribution_center_name\" : \"NEW DC\",\"email\" : \"\",\"fax\" : \"\",\"phone\" : \"\",\"postal_code\" : \"12345\",\"state\" : \"CA\",\"website\" : \"\"}]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}
void get_websites_param_func(const std::shared_ptr< Session > session)
{
	/*stringstream id;
	id << ::this_thread::get_id();
	auto body = String::format("Thread:%s,Password Protected! \n", id.str().data());*/

	const auto request = session->get_request();
	auto ret = request->get_query_parameters();
	for (auto& r : ret)
		cout << r.first << ":" << r.second << endl;
	string body = "[ { \"Website_Id\" : 35,  \"website_name\" : \"DTC Frontend\"  } ]";
	session->close(OK, body, { { "Content-Length", ::to_string(body.length()) } });
}

std::string&   replace_all(std::string&   str,const   std::string&   old_value,const   std::string&   new_value)     
{     
    while(true)   {     
        std::string::size_type   pos(0);     
        if(   (pos=str.find(old_value))!=std::string::npos   )     
            str.replace(pos,old_value.length(),new_value);     
        else   break;     
    }     
    return   str;     
}     
std::string&   replace_all_distinct(std::string&   str,const   std::string&   old_value,const   std::string&   new_value)     
{     
    for(std::string::size_type   pos(0);   pos!=std::string::npos;   pos+=new_value.length())   {     
        if(   (pos=str.find(old_value,pos))!=std::string::npos   )     
            str.replace(pos,old_value.length(),new_value);     
        else   break;     
    }     
    return   str;     
} 


#endif	