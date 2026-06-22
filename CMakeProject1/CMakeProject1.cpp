#include <dpp/dpp.h>
#include "Token.h"


struct Fuel
{
	float number_laps;
	int number_laps_int, number_stints;
	std::vector<double> fuel_stint;
	std::vector<double> energy_stint;
	
	Fuel(double number_laps, int number_laps_int,int number_stints, std::vector<double> fuel_stint, std::vector<double> energy_stint) {
		this->energy_stint = energy_stint;
		this->fuel_stint = fuel_stint;
		this->number_laps = number_laps;
		this->number_laps_int = number_laps_int;
		this->number_stints = number_stints;
	}


};

Fuel fuel_calculate(const dpp::slashcommand_t& event) {	// kalkulator paliwa 

		//logika
		int length = std::get<int64_t>(event.get_parameter("length"));						// zebranie potrzebnych danych
		int tank = std::get<int64_t>(event.get_parameter("tank"));

		double fuel_usage = std::get<double>(event.get_parameter("fuel_usage"));

		std::string lap_time = std::get<std::string>(event.get_parameter("lap_time"));
		length = length * 60;		// zrzucenie z min na sec
																					// zrzucenie z x.xx.xxx na xx.xxx sec
		int temp_min = (int)(lap_time.at(0) - '0');									// wzięcie minut 
		double lap = std::stof(lap_time.substr(lap_time.find('.'))) + temp_min * 60;	// wyjęcie sekund i milisekund oraz dodanie minut zamienione na sec

		int buffor_laps = 0;
		if (event.get_parameter("bufor_laps").index() != 0) {				// pobieram zużycie energii jeśli użytkownik podał i tutaj obliczam ilość stintów
			buffor_laps = std::get<int64_t>(event.get_parameter("bufor_laps"));
		}
		float number_laps = (length / lap) + buffor_laps;
		int number_laps_int = std::floor(number_laps);

		double energy_usage = 0;
		int number_stints = 0;
		if (event.get_parameter("energy_usage").index() != 0) {				// pobieram zużycie energii jeśli użytkownik podał i tutaj obliczam ilość stintów
			energy_usage = std::get<double>(event.get_parameter("energy_usage"));

			number_stints = std::ceil(number_laps_int / (100 / energy_usage));

		}
		else {

			number_stints = std::ceil(number_laps_int / (tank / fuel_usage));

		}

		std::vector<double> fuel_stint;
		double race_fuel = number_laps_int * fuel_usage;		// obliczenie ile paliwa na cały wyścig
		double race_energy = number_laps_int * energy_usage;
		std::vector<double> energy_stint;

		if (race_energy > 100) {	// jeśli jest wirtualna energia obliczam ile jej potrzebuję a potem obliczam paliwo
			int temp_energy = 0;
			int laps_per_stint = 100 / energy_usage;
			for (int i = 0; i < number_stints - 1; i++) {
				energy_stint.push_back(100);
				temp_energy += 100;
				fuel_stint.push_back(laps_per_stint * fuel_usage);
			}
			energy_stint.push_back(race_energy - temp_energy);
			laps_per_stint = (race_energy - temp_energy) / energy_usage;
			fuel_stint.push_back(laps_per_stint * fuel_usage);

		}
		else if (race_energy == 0) {	// jeśli nie ma virutalnej energi obliczam na podstawie paliwa
			if (race_fuel > tank) {
				double temp_fuel = 0;
				for (int i = 0; i < number_stints - 1; i++) {
					fuel_stint.push_back(tank);
					temp_fuel += tank;
				}
				fuel_stint.push_back(race_fuel - temp_fuel);
			}
			else {
				fuel_stint.push_back(race_fuel);
			}
		}
		else {		// w przypadku jeśli jest wirtualna energia 
			energy_stint.push_back(race_fuel);
		}




	Fuel fuel(number_laps, number_laps_int, number_stints , fuel_stint, energy_stint);
	return fuel;

}




int main() {
	dpp::cluster bot(BOT_TOKEN, dpp::i_default_intents | dpp::i_message_content);

	bot.on_log(dpp::utility::cout_logger());

	bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
		if (event.command.get_command_name() == "kalkulator-paliwa") {	// kalkulator paliwa z użyciem embed
			Fuel fuel = fuel_calculate(event);

			dpp::embed embed = dpp::embed();

			{													// ustawienia embed
				embed.set_color(0x7b0323);
				embed.set_title("Kalkulator Paliwa");
				embed.set_author("MPL Bot", "", "https://www.mplmotorsport.pl/MPL_logo.png");

				embed.set_timestamp(time(0));
				embed.set_footer(
					dpp::embed_footer()
					.set_text("By Piluszek")
					.set_icon("https://www.mplmotorsport.pl/MPL_logo.png")
				);

				std::string number_laps_string = std::to_string(fuel.number_laps);
				std::string tekst = (std::to_string(fuel.number_laps_int) + " okrazen ""(" + number_laps_string.substr(0, number_laps_string.find('.')+2) + ")");
				embed.add_field(
					"Ilosc okrazen:",
					tekst
				);

				std::string number_stints_string = std::to_string(fuel.number_stints);
				embed.add_field(
					"Ilosc stintow:",
					number_stints_string
				);

				if (fuel.energy_stint[0] == NULL) {
					for (int i = 0; i <= fuel.fuel_stint.size()-1; i++) {
						std::string fuel_string = std::to_string(fuel.fuel_stint[i]);
						std::string tekst = (fuel_string).substr(0, fuel_string.find('.') + 2) + " L"; // tutaj tymaczasowo zaokrągla w górę !!!!
						std::string tekst2 = "Stint " + std::to_string(i+1)+":";
						embed.add_field(
							tekst2,
							tekst
						);
					}
				}
				else {
					for (int i = 0; i <= fuel.fuel_stint.size() - 1; i++) {
						std::string fuel_string = std::to_string(fuel.fuel_stint[i]);
						std::string energy_string = std::to_string(fuel.energy_stint[i]);
						std::string tekst = (fuel_string).substr(0, fuel_string.find('.') + 2) + " L" + " (" + 
						(energy_string).substr(0, energy_string.find('.') + 2) + " %)"; // tutaj tymaczasowo zaokrągla w górę !!!!
						
						std::string tekst2 = "Stint " + std::to_string(i + 1) + ":";
						embed.add_field(
							tekst2,
							tekst
						);
					}
				
				
				}

			}

			dpp::message msg(event.command.channel_id, embed);

			event.reply(msg);
		}
		
		
		if (event.command.get_command_name() == "dm") {

			dpp::snowflake user;

			if (event.get_parameter("user").index() == 0) {
				user = event.command.get_issuing_user().id;
			}
			else {
				user = std::get<dpp::snowflake>(event.get_parameter("user"));
			}

			bot.direct_message_create(user, dpp::message("czesc c-wordzie!"));

			

		}




		});


	bot.on_ready([&bot](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_bot_commands>()) {

			std::vector<dpp::slashcommand> commands;
			
			dpp::slashcommand calculator("kalkulator-paliwa", "Kalkulator Paliwa", bot.me.id);
			calculator.add_option(dpp::command_option(dpp::co_integer, "length", "dlugosc wyscigu(min)", true));
			calculator.add_option(dpp::command_option(dpp::co_string, "lap_time", "dlugosc okrazania(x.xx.xxx)", true));
			calculator.add_option(dpp::command_option(dpp::co_integer, "tank", "pojemnosc zbiornika paliwa (l)", true));
			calculator.add_option(dpp::command_option(dpp::co_number, "fuel_usage", "zuzycie paliwa l/lap", true));
			calculator.add_option(dpp::command_option(dpp::co_number, "energy_usage", "zuzycie energii (%/lap)", false));
			calculator.add_option(dpp::command_option(dpp::co_integer, "bufor_laps", "ile lapow zapasu", false));
			
			commands.push_back(calculator);
			
			commands.push_back(dpp::slashcommand("porownaj-czas", "Porownuje czas z czasami esportowcow", bot.me.id));
			
			dpp::slashcommand dm("dm", "dm", bot.me.id);
			dm.add_option(dpp::command_option(dpp::co_mentionable, "user", "The user to message", false));
			commands.push_back(dm);

			uint64_t serwer_id = 1517580367631220837;					// zmienić potem - usunąć tą zmienną
			bot.guild_bulk_command_create(commands, serwer_id);			// zmienić potem - zmienić na global bulk
		}
		});

	bot.start(dpp::st_wait);
}
