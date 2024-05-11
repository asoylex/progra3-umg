#include <iostream>
#include <string>
#include <curl/curl.h>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <AVFoundation/AVFoundation.h>


// Función estática para manejar los datos recibidos por cURL
static size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

// Función para realizar una solicitud HTTP utilizando cURL
std::string makeHttpRequest(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback); // Utiliza la función estática
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error al realizar la solicitud HTTP: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    return response;
}

int main() {
    std::string apiKey = "AIzaSyAEMUbDuImkLj9olXF-Mya0G1v2Vowd6uQ";
    std::string word, language, option;

    std::cout << "Ingrese la palabra que desea traducir: ";
    std::cin >> word;

    std::cout << "Ingrese el idioma (Español: es, Inglés: en, Francés: fr): ";
    std::cin >> language;

    
    std::cout << "Ingrese la opción (audio, texto): ";
    std::cin >> option;

    std::string url = "https://translation.googleapis.com/language/translate/v2?key=" + apiKey +
                      "&q=" + word + "&target=" + language;

    std::string response = makeHttpRequest(url);
    std::string pos = extractTranslatedText(response);
    std::cout << "Respuesta de la API: " << pos << std::endl;

     if (option == "audio") {
        // Convertir la cadena de texto en voz
        NSString *texto = [NSString stringWithCString:pos.c_str() encoding:NSUTF8StringEncoding];
        AVSpeechUtterance *utterance = [[AVSpeechUtterance alloc] initWithString:texto];
        AVSpeechSynthesizer *synthesizer = [[AVSpeechSynthesizer alloc] init];
        
        // Obtener la identificación de voz adecuada para el idioma seleccionado
        NSString *voiceIdentifier = voiceIdentifierForLanguage(language);
        AVSpeechSynthesisVoice *voice = [AVSpeechSynthesisVoice voiceWithIdentifier:voiceIdentifier];
        utterance.voice = voice;
        
        [synthesizer speakUtterance:utterance];
        
        // Esperar hasta que se complete la síntesis de voz
        while ([synthesizer isSpeaking]) {
            // Esperar...
        }
    } 
    return 0;
}

