
#include "httplib.h"
#include <iostream>
#include <ctime>
#include <string>


std::string pobierzPogode(const std::string& miasto) {

    httplib::Client cli("http://wttr.in");
    cli.set_connection_timeout(0, 500000);
    cli.set_read_timeout(5, 0);


    std::string path = "/" + miasto + "?format=3";

    if (auto res = cli.Get(path)) {
        if (res->status == 200) {
            return res->body;
        } else {
            return "Blad serwera pogodowego. Kod: " + std::to_string(res->status);
        }
    }
    return "Nie udalo sie polaczyc z serwisem pogodowym.";
}

int main() {

    std::time_t t = std::time(nullptr);
    std::cout << "Data uruchomienia: " << std::asctime(std::localtime(&t));
    std::cout << "Autor: Tomasz Duchnik" << std::endl;
    std::cout << "Nasluchuje na porcie TCP: 8080" << std::endl;

    httplib::Server svr;


    svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
        std::string html = R"(
            <html>
                <head>
                    <meta charset="UTF-8">
                    <title>Pogoda w Chmurze</title>
                    <style>
                        body { font-family: Arial, sans-serif; margin: 40px; }
                        select, input { padding: 10px; margin-top: 10px; }
                    </style>
                </head>
                <body>
                    <h2>Wybierz lokalizacje</h2>
                    <form action="/pogoda" method="GET">
                        <select name="miasto">
                            <option value="Warsaw">Warszawa, Polska</option>
                            <option value="Lublin">Lublin, Polska</option>
                            <option value="London">Londyn, UK</option>
                            <option value="Tokyo">Tokio, Japonia</option>
                        </select>
                        <br>
                        <input type="submit" value="Sprawdz aktualna pogode">
                    </form>
                </body>
            </html>
        )";
        res.set_content(html, "text/html");
    });

    // Obsługa formularza i wyświetlenie wyniku
    svr.Get("/pogoda", [](const httplib::Request &req, httplib::Response &res) {
        std::string miasto = req.has_param("miasto") ? req.get_param_value("miasto") : "Warsaw";

        // Wywołanie naszej funkcji odpytującej zewnetrzne API
        std::string wynik_pogody = pobierzPogode(miasto);

        std::string html = R"(
            <html>
                <head>
                    <meta charset="UTF-8">
                    <title>Wynik Pogodowy</title>
                    <style>body { font-family: Arial, sans-serif; margin: 40px; }</style>
                </head>
                <body>
                    <h2>Aktualna pogoda:</h2>
                    <p style="font-size: 24px; font-weight: bold; color: #0066cc;">)" + wynik_pogody + R"(</p>
                    <br><br>
                    <a href="/" style="text-decoration: none; padding: 10px 15px; background: #eee; border-radius: 5px;">Powrot do formularza</a>
                </body>
            </html>
        )";
        res.set_content(html, "text/html");
    });

    // Uruchomienie serwera na wszystkich adresach IP kontenera
    std::cout << "Serwer HTTP gotowy " << std::endl;
    svr.listen("0.0.0.0", 8080);
}