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
std::string extractTranslatedText(const std::string& jsonResponse) {
    // Busca la posición de "translatedText"
    size_t pos = jsonResponse.find("\"translatedText\"");

    // Verifica si se encontró la cadena
    if (pos == std::string::npos) {
        std::cerr << "No se encontró la clave 'translatedText' en la respuesta JSON" << std::endl;
        return "";
    }

    // Busca el inicio del valor de "translatedText"
    size_t start = jsonResponse.find(':', pos);
    if (start == std::string::npos) {
        std::cerr << "Error al encontrar el inicio del valor de 'translatedText'" << std::endl;
        return "";
    }

    // Busca el final del valor de "translatedText"
    size_t end = jsonResponse.find_first_of(",}", start);
    if (end == std::string::npos) {
        std::cerr << "Error al encontrar el final del valor de 'translatedText'" << std::endl;
        return "";
    }

    // Extrae el valor de "translatedText"
    std::string translatedText = jsonResponse.substr(start + 3, end - start - 4); // +3 y -4 para eliminar comillas y espacios

    return translatedText;
}
NSString* voiceIdentifierForLanguage(const std::string& language) {
    // Mapear el idioma a la identificación de voz adecuada
    if (language == "es") {
        return @"com.apple.speech.synthesis.voice.diego";
    } else if (language == "en") {
        return @"com.apple.speech.synthesis.voice.samantha";
    } else if (language == "fr") {
        return @"com.apple.speech.synthesis.voice.thomas";
    }
    // Por defecto, devolver la voz en inglés
    return @"com.apple.speech.synthesis.voice.samantha";
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

