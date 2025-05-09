#include <cpr/cpr.h>
#include <crow.h>

int main() {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/pfp")
  ([](const crow::request &req, crow::response &res) {
    // Get the url GET parameter from the req
    auto url = req.url_params.get("url");
    // Fetch the response of the specified URL
