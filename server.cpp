////////////////////////////////////////////////////////////////////////////////

#include "server.h"
#include "producto.h"
#include "commands.h"

#include <WebServer.h>

////////////////////////////////////////////////////////////////////////////////

static Producto *producto;
static WebServer server(6969);

////////////////////////////////////////////////////////////////////////////////

static void post_handler();
static void get_root();
static void not_found();

////////////////////////////////////////////////////////////////////////////////

void server_init(Producto *p)
{
  producto = p;
  
  /* Main Webpage */
  server.on("/", HTTP_GET, get_root);

  /* POST Handlers */
  server.on("/", HTTP_POST, post_handler);

  /* GET Handlers */
  // server.on("/TASK", HTTP_GET, get_test);

  /* Invalid request */
  server.onNotFound(not_found);
}

void server_start()
{
  server.begin();
  Serial.println("HTTP server started");
}

void server_check_requests()
{
  server.handleClient();
}

////////////////////////////////////////////////////////////////////////////////

static void post_handler()
{
  String resp = "Done.";
  
  // Request must have a method field
  if (!server.hasArg("method")) {
    goto Error;
  }

  if (server.arg("method") == "read_task_history") {
    print_task_history();
  }

  else if (server.arg("method") == "delete_task_history") {
    delete_task_history();
  }

  else if (server.arg("method") == "get_current_task") {
    resp = get_current_task(producto);
  }

  else if (server.arg("method") == "current_task_states") {
    resp = get_active_file_contents();
  }

  else if (server.arg("method") == "get_task_history") {
    // Serial.println("AAAAAAAAA");
    resp = get_task_file_contents();
    // Serial.println("BBBBBBBBBB");
    resp += get_active_file_contents();
    // Serial.println("CCCCCCCCCCC");
  }

  else {
    goto Error;
  }
  
  server.send(200, "text/plain", resp);
  return;

 Error:
  server.send(404, "text/plain", "400: Bad request");
  return;
}

static void get_root()
{
  Serial.println("Handling ROOT request...");
	
  String root_html = "<html>"
    "POOP"
    "</html>";	
	
  server.send(200, "text/html", root_html);
}

static void not_found()
{
  Serial.println("Handling INVALID request...");	
  server.send(404, "text/plain", "404: Not found");
}

////////////////////////////////////////////////////////////////////////////////
