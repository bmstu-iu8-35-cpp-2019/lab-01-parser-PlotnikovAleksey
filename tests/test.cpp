// Copyright 2019 Plotnikov Aleksey <alex280201@gmail.com>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <header.hpp>

TEST(Json, Construct) {
  std::string text =
      "{\"orderID\": 12345,\"shopperName\": \"Ivan\",\"shopperEmail\" : "
      "\"ivanov@example.com\",\"contents\" : [{\"productID\": "
      "34,\"productName\": \"super\",\"quantity\": 1},{\"productID\": "
      "56,\"productName\" : \"super-duper\",\"quantity\" : 3}, "
      "true],\"orderCompleted\": true}";
  Json obj(text);
  EXPECT_EQ(obj.is_array(), false);
  EXPECT_EQ(obj.is_object(), true);
  EXPECT_EQ(std::any_cast<double>(obj["orderID"]), 12345.);
  EXPECT_EQ(std::any_cast<std::string>(obj["shopperName"]), "Ivan");
  EXPECT_EQ(std::any_cast<std::string>(obj["shopperEmail"]),
            "ivanov@example.com");

  auto cont = std::any_cast<Json>(obj["contents"]);

  EXPECT_EQ(cont.is_array(), true);
  EXPECT_EQ(cont.is_object(), false);
  EXPECT_EQ(std::any_cast<bool>(cont[2]), true);

  auto prod1 = std::any_cast<Json>(cont[0]);
  auto prod2 = std::any_cast<Json>(cont[1]);

  EXPECT_EQ(prod1.is_array(), false);
  EXPECT_EQ(prod1.is_object(), true);
  EXPECT_EQ(prod2.is_array(), false);
  EXPECT_EQ(prod2.is_object(), true);
  EXPECT_EQ(std::any_cast<double>(prod1["productID"]), 34.);
  EXPECT_EQ(std::any_cast<std::string>(prod1["productName"]), "super");
  EXPECT_EQ(std::any_cast<double>(prod1["quantity"]), 1.);
  EXPECT_EQ(std::any_cast<double>(prod2["productID"]), 56.);
  EXPECT_EQ(std::any_cast<std::string>(prod2["productName"]), "super-duper");
  EXPECT_EQ(std::any_cast<double>(prod2["quantity"]), 3.);
}

TEST(Json, parse) {
  std::string text =
      "[ 1.4567, 50.67, null,{ \"Having fun?\":\"Defenetly yes\" }]";
  Json obj = Json::parse(text);

  EXPECT_EQ(obj.is_array(), true);
  EXPECT_EQ(obj.is_object(), false);
  EXPECT_EQ(std::any_cast<double>(obj[0]), 1.4567);
  EXPECT_EQ(std::any_cast<double>(obj[1]), 50.67);
  EXPECT_EQ(obj[2].has_value(), false);

  auto t1 = std::any_cast<Json>(obj[3]);
  EXPECT_EQ(t1.is_array(), false);
  EXPECT_EQ(t1.is_object(), true);
  EXPECT_EQ(std::any_cast<std::string>(t1["Having fun?"]), "Defenetly yes");
}

TEST(Json, parsefile) {
	Json obj = Json::parseFile("../myfile.json");

	EXPECT_EQ(std::any_cast<double>(obj["ID"]), 322228.);
    EXPECT_EQ(std::any_cast<std::string>(obj["SortAs"]), "SGML");
    EXPECT_EQ(std::any_cast<std::string>(obj["GlossTerm"]),
                  "Standard Generalized Markup Language");
    EXPECT_EQ(std::any_cast<std::string>(obj["Acronym"]), "SGML");
    EXPECT_EQ(std::any_cast<std::string>(obj["Abbrev"]), "ISO 8879:1986");
}

TEST(Json, exceptions) {
  std::string wrong_text1 =
      "[ 1.4567 50.67, null,{ \"Having fun?\":\"Defenetly yes\" }]";
  std::string wrong_text2 =
      " 1.4567, 50.67, null,{ \"Having fun?\":\"Defenetly yes\" }]";
  std::string wrong_text3 =
      "{\"orderID\" 12345,\"shopperName\": \"Ivan\",\"shopperEmail\" : "
      "\"ivanov@example.com\",\"orderCompleted\": true}";
  std::string wrong_text4 =
      "{\"orderID\": 12345,\"shopperName\": \"Ivan\",\"shopperEmail\" : "
      "ivanov@example.com,\"orderCompleted\": true}";

  EXPECT_THROW(auto a = Json(wrong_text1), std::invalid_argument);
  EXPECT_THROW(auto b = Json(wrong_text2), std::invalid_argument);
  EXPECT_THROW(auto c = Json(wrong_text3), std::invalid_argument);
  EXPECT_THROW(auto d = Json(wrong_text4), std::invalid_argument);
}

TEST(json, parse_tickers) {
  json obj = {{"Si-9.15", "RTS-9.15", "GAZP-9.15"},
              {100024, 100027, 100050},
              {"Futures contract for USD/RUB", "Futures contract for index RTS",
               "Futures contract for Gazprom shares"}};
  auto res = parse_tickers(obj);

  EXPECT_EQ(res[0]["ticker"], "Si-9.15");
  EXPECT_EQ(res[1]["ticker"], "RTS-9.15");
  EXPECT_EQ(res[2]["ticker"], "GAZP-9.15");
  EXPECT_EQ(res[0]["id"], 100024);
  EXPECT_EQ(res[1]["id"], 100027);
  EXPECT_EQ(res[2]["id"], 100050);
  EXPECT_EQ(res[0]["description"], "Futures contract for USD/RUB");
  EXPECT_EQ(res[1]["description"], "Futures contract for index RTS");
  EXPECT_EQ(res[2]["description"], "Futures contract for Gazprom shares");
}
