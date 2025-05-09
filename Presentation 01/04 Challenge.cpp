// You are a student/hacker looking to change your grade through hacking the university's web app

/*
Your legitimate API request looks like this:
GET /grades?studentid=20223948&subjectid=1293 HTTP/2
Host: api.grades.example.com 

What is your first instinct as a hacker to poke?
*/

#include <cpprest/http_listener.h>
#include <cpprest/json.h>

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

void handle_get(http_request request) {
    auto grade = json::value::object();
    auto lookup = json::value::object();

    // Get query parameters from request URI
    auto uri_query = uri::split_query(request.request_uri().query());

    // Extract studentID and subjectID from query parameters
    lookup["studentID"] = json::value::string(uri_query["studentid"]);
    lookup["subjectID"] = json::value::string(uri_query["subjectid"]);

    // Lookup grade in the database using extracted studentID and subjectID
    grade = lookupGradeInDB(lookup);

    // Send the grade as the response
    request.reply(status_codes::OK, grade);
}

void handle_patch(http_request request) {
    auto update = json::value::object();
    auto response = json::value::object();

    // Read JSON values
    request.extract_json().then([&update](pplx::task<json::value> task) {
        auto jsonBody = task.get();
        update["grade"] = jsonBody.at("grade");
        update["studentID"] = jsonBody.at("studentID");
        update["subjectID"] = jsonBody.at("subjectID");
    }).wait();

    // Update grade in the database
    response = updateGradeInDB(update);

    request.reply(status_codes::OK, response);
}

int main() {
    // Initialize HTTP listener
    http_listener listener("http://0.0.0.0:443/grades");

    // Register handlers for GET and PATCH methods
    listener.support(methods::GET, handle_get);
    listener.support(methods::PATCH, handle_patch);

    // Start the listener
    listener.open().wait();
    std::cout << "Server started..." << std::endl;
    std::cout << "Listening on " << listener.uri().to_string() << std::endl;

    // Wait for incoming requests
    while (true);
    return 0;
}
