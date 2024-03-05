#include <Wire.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "TuSSID";
const char *password = "TuContraseñaWiFi";

const char *db_host = "tu_servidor_mysql";
const char *db_user = "tu_usuario_mysql";
const char *db_password = "tu_contraseña_mysql";
const char *db_name = "tu_base_de_datos";

AsyncWebServer server(80);

MySQL_Connection conn((Client *)&client);

void handleRoot(AsyncWebServerRequest *request) {
  String html = "<html><body>";
  html += "<form action='/submit' method='post'>";
  html += "Usuario: <input type='text' name='username'><br>";
  html += "Contraseña: <input type='password' name='password'><br>";
  html += "<input type='submit' value='Enviar'>";
  html += "</form></body></html>";
  request->send(200, "text/html", html);
}

void handleSubmit(AsyncWebServerRequest *request) {
  String username = request->arg("username");
  String password = request->arg("password");

  if (connectToMySQL()) {
    if (verifyCredentials(username, password)) {
      // Credenciales correctas, activar el relé
      // ...
      request->send(200, "text/plain", "Credenciales correctas. Relé activado.");
    } else {
      request->send(401, "text/plain", "Credenciales incorrectas.");
    }
    conn.close();
  } else {
    request->send(500, "text/plain", "Error de conexión a la base de datos.");
  }
}

bool connectToMySQL() {
  if (conn.connect(db_host, db_user, db_password)) {
    Serial.println("Conectado a MySQL.");
    return true;
  } else {
    Serial.println("Error de conexión a MySQL.");
    return false;
  }
}

bool verifyCredentials(const String &username, const String &password) {
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

  // Realizar la consulta SQL para verificar las credenciales
  String query = "SELECT * FROM usuarios WHERE username='" + username + "' AND password='" + password + "'";
  cur_mem->execute(query);

  if (cur_mem->rowsCount() > 0) {
    Serial.println("Credenciales verificadas.");
    delete cur_mem;
    return true;
  } else {
    Serial.println("Credenciales incorrectas.");
    delete cur_mem;
    return false;
  }
}

void setup() {
  Serial.begin(115200);

  // Conexión a WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  // Rutas del servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);

  // Inicializar servidor
  server.begin();
}

void loop() {
  // Puedes agregar aquí cualquier otra lógica que necesites
}
