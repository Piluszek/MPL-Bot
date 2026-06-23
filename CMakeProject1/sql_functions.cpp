#include <string>      
#include <functional>
#include <mysql/mysql.h>
std::string connect(std::function<std::string(MYSQL*)> funkcja ) {
	MYSQL* conn = mysql_init(nullptr);
	mysql_real_connect(conn, "127.0.0.1", "root", "", "mpl_bot", 3306, nullptr, 0);
		

	std::string function_string = funkcja(conn);
	mysql_close(conn);
	return function_string;

}

void connectVoid(std::function<void(MYSQL*)> funkcja) {
	MYSQL* conn = mysql_init(nullptr);
	mysql_real_connect(conn, "127.0.0.1", "root", "", "mpl_bot", 3306, nullptr, 0);

	funkcja(conn);
	mysql_close(conn);

}

void addteam(MYSQL* conn, const std::string &team_name, const std::string &server_id) {
	std::string query= "INSERT INTO `teams`(`server_id`, `team_name`, `points`) VALUES('" + server_id + "', '" + team_name + "', '" + "0" + "')";
	std::string test_query = "SELECT * FROM `teams` WHERE `server_id` = '" + server_id + "' AND `team_name` = '" + team_name + "'";
	if (mysql_query(conn, test_query.c_str()) == 0) {
		MYSQL_RES* result = mysql_store_result(conn); // przechowuje dane z zapytania
		mysql_free_result(result);
		if (result != nullptr) {
			if (mysql_num_rows(result) > 0) {
				throw "Team_exist";
			}
			else {
				if (mysql_query(conn, query.c_str()) == 0) {
				}
				else {
					throw "B³¹d zapytania";
				}
			}
		}
	}
	else {
		throw "B³¹d zapytania";
	}
	
	
}