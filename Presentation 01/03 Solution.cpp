void handle_get(http_request request) {
    auto current_user = getCurrentUser(); 
    if (!current_user) {
        request.reply(status_codes::Unauthorized);
        return;
    }

    auto grade = json::value::object();
    auto lookup = json::value::object();

    // Get query parameters from request URI
    auto uri_query = uri::split_query(request.request_uri().query());
    lookup["studentID"] = json::value::string(uri_query["studentid"]);
    lookup["subjectID"] = json::value::string(uri_query["subjectid"]);

    // Access control
    if (lookup["studentID"] != current_user.studentID) {
        request.reply(status_codes::Forbidden);
        return;
    }

    // Lookup grade in the database using extracted studentID and subjectID
    grade = lookupGradeInDB(lookup);

    request.reply(status_codes::OK, grade);
}



void handle_patch(http_request request) {
    auto current_user = getCurrentUser(); 
    if (!current_user) {
        request.reply(status_codes::Unauthorized);
        return;
    }

    auto update = json::value::object();
    auto response = json::value::object();

    // Read JSON values
    request.extract_json().then([&update](pplx::task<json::value> task) {
        auto jsonBody = task.get();
        update["grade"] = jsonBody.at("grade");
        update["studentID"] = jsonBody.at("studentID");
        update["subjectID"] = jsonBody.at("subjectID");
    }).wait();

    // Access control
    if (current_user.role != "teacher") {
        request.reply(status_codes::Forbidden);
        return;
    }

    // Update grade in the database
    response = updateGradeInDB(update);

    request.reply(status_codes::OK, response);
}
