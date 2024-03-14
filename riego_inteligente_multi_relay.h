#include "esphome.h"
using namespace esphome;

class RiegoInteligenteRelay {
 public:
  RiegoInteligenteRelay(GPIOPin *pin, uint32_t intervalo_inicial, uint32_t duracion_activacion_inicial, uint32_t cantidad_activaciones_inicial, uint32_t intervalo_final, uint32_t duracion_activacion_final, uint32_t cantidad_activaciones_final)
      : pin_(pin), intervalo_inicial_(intervalo_inicial), duracion_activacion_inicial_(duracion_activacion_inicial), cantidad_activaciones_inicial_(cantidad_activaciones_inicial), intervalo_final_(intervalo_final), duracion_activacion_final_(duracion_activacion_final), cantidad_activaciones_final_(cantidad_activaciones_final) {
        pin_->setup();
        intervalo_actual_ = intervalo_inicial_;
        duracion_actual_ = duracion_activacion_inicial_;
        fase_ = 0; // Iniciar en la primera fase
        activado_ = false;
        contador_activaciones_ = 0;
        previousMillis_ = 0;
      }

  void controlRelay() {
    unsigned long currentMillis = millis();
    if (!activado_ && currentMillis - previousMillis_ >= intervalo_actual_) {
      activado_ = true;
      previousMillis_ = currentMillis;
      pin_->digital_write(true);
      tiempo_activacion_ = millis();
    }
    if (activado_ && currentMillis - tiempo_activacion_ >= duracion_actual_) {
      activado_ = false;
      pin_->digital_write(false);
      contador_activaciones_++;
      ajustarFase();
    }
  }

 private:
  GPIOPin *pin_;
  uint32_t intervalo_inicial_, duracion_activacion_inicial_, cantidad_activaciones_inicial_;
  uint32_t intervalo_final_, duracion_activacion_final_, cantidad_activaciones_final_;
  uint32_t intervalo_actual_, duracion_actual_;
  bool activado_;
  unsigned long previousMillis_, tiempo_activacion_;
  uint32_t contador_activaciones_;
  int fase_; // 0 para la primera fase, 1 para la segunda fase

  void ajustarFase() {
    if (contador_activaciones_ == cantidad_activaciones_inicial_ && fase_ == 0) {
      fase_ = 1;
      intervalo_actual_ = intervalo_final_;
      duracion_actual_ = duracion_activacion_final_;
    } else if (contador_activaciones_ >= cantidad_activaciones_inicial_ + cantidad_activaciones_final_ && fase_ == 1) {
      // Opción para finalizar operaciones o reiniciar según necesidad
    }
  }
};

class RiegoInteligenteMultiRelay : public Component {
 public:
  RiegoInteligenteMultiRelay() {}

  void add_relay(RiegoInteligenteRelay *relay) {
    relays_.push_back(relay);
  }

  void setup() override {
    // La inicialización individual de los relés ya se maneja en RiegoInteligenteRelay
  }

  void loop() override {
    for (auto *relay : relays_) {
      relay->controlRelay();
    }
  }

 private:
  std::vector<RiegoInteligenteRelay *> relays_;
};
