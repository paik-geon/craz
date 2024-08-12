#include <Adafruit_Fingerprint.h> // 지문 인식 센서를 사용하기 위한 Adafruit Fingerprint 라이브러리 호출
#include <SoftwareSerial.h> // 추가적인 시리얼 통신을 위해 SoftwareSerial 라이브러리 호출
uint8_t id; // ID라는 8비트 정수형 변수를 선언
boolean operate_state = false; // 운영 상태를 나타내는 boolean 변수를 false 상태로 선언
SoftwareSerial mySerial(2, 3); // SoftwareSerial 객체를 생성하여 이름을 mySerial로 설정 - 2번 핀을 RX, 3번 핀을 TX로 사용
int temp = 0; // 임시로 사용할 정수형 변수를 0으로 선언

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); // 지문 인식 센서를 제어하기 위해 Adafruit_Fingerprint 객체 생성, mySerial을 통해 통신

void setup()  
{
  Serial.begin(9600); // 기본 시리얼 통신 속도를 9600bps로 설정
  while (!Serial);  // Serial 포트가 준비될 때까지 대기 (Yun/Leo/Micro/Zero 보드에서 사용)
  delay(100); // 100ms 지연
  Serial.println("\n\nAdafruit finger detect test"); // 시작 메세지를 시리얼 모니터에 출력

  finger.begin(57600); // 지문 인식 센서의 데이터 전송 속도를 57600bps로 설정

  // 아래 주석처리된 코드는 센서의 패스워드를 확인 - 만약 패스워드가 맞지 않으면 오류 메시지를 출력하고 프로그램을 멈춤
  /*
  if (finger.verifyPassword()) {
   Serial.println("Found fingerprint sensor!");
   } 
   else {
   Serial.println("Did not find fingerprint sensor 😞");
   while (1) { 
   delay(1); 
   }
   }*/

  pinMode(12,OUTPUT); // 12번 핀을 출력 모드로 설정
  finger.getTemplateCount(); // 센서에 저장된 지문 템플릿 수를 가져옴
  Serial.print("Sensor contains "); // print문 출력
  Serial.print(finger.templateCount); // 센서에 저장된 지문 템플릿 수를 시리얼모니터에 출력
  Serial.println(" templates"); // print문 출력
  Serial.println("Waiting for valid finger..."); // 유효한 지문을 기다리고 있음을 시리얼모니터에 출력
}

int state = 0, lock = 0; // state와 lock을 정수형 변수 0으로 선언

boolean sw(int pin, int direction){ // 주어진 핀에서 읽은 아날로그 값에 따라 스위치 상태를 반환하는 함수
  if(direction){ // 만약 direction이 true인 경우
    if(analogRead(pin) < 100) return false; // 아날로그 값이 100 미만이면 false 반환
    else return true; // 그렇지 않으면 true 반환
  }
  
  else { // 만약 direction이 false인 경우
    if(analogRead(pin) < 100) return true; // 아날로그 값이 100 미만이면 true 반환
    else return false; // 그렇지 않으면 false 반환
  }
}

void loop() // 무한반복
{
  if(sw(0, true)) state = 0; // 핀 0이 true이면 상태를 0으로 선언
  if(sw(1, true)) state = 1; // 핀 1이 true이면 상태를 1로 선언
  if(sw(2, true)) state = 2; // 핀 2가 true이면 상태를 2로 선언
  
  if(getFingerprintIDez() == 1) lock = true; // 지문 인식이 성공하면 lock을 true로 설정
  if(state == 0){
    enroll_action(); // state가 0이면 지문 등록 절차 수행
    state = -1; // state를 -1로 설정하여 등록 절차가 완료됨을 표시
    lock = 0; // 잠금 해제
  }
  else if(state == 1) digitalWrite(12, HIGH); // state가 1이면 12번 핀에 HIGH 신호를 출력 - LED ON
  else if(state == 2) digitalWrite(12, LOW); // state가 2이면 12번 핀에 LOW 신호를 출력 - LED OFF
  
  if(state == 1 && lock) state = 2; // state가 1이고 잠금이 걸려있으면 상태를 2로 변경
  else if(state == 2 && lock) lock = 0; // state가 2이고 잠금이 걸려있으면 잠금을 해제
  Serial.println(state); // 현재 State를 시리얼 모니터에 출력
}

void enroll_action(){ // 지문을 등록하는 함수
  Serial.println("Ready to enroll a fingerprint!"); // 지문 등록 준비 메세지 출력
  id = 1; // 등록할 지문 ID를 1로 설정
  Serial.print("Enrolling ID #");
  Serial.println(id); // 등록할 지문 ID를 시리얼 모니터에 출력

  while (!  getFingerprintEnroll() ); // 지문 등록이 완료될 때까지 대기
}

uint8_t getFingerprintID() { // 지문을 인식하고 ID를 반환하는 함수
  uint8_t p = finger.getImage(); // 지문 이미지를 캡처
  switch (p) { 
  case FINGERPRINT_OK: // 지문 이미지 캡처 성공
    Serial.println("Image taken");
    break;
  case FINGERPRINT_NOFINGER: // 지문이 감지되지 않음
    Serial.println("No finger detected");
    return p;
  case FINGERPRINT_PACKETRECIEVEERR: // 통신 오류 발생
    Serial.println("Communication error");
    return p;
  case FINGERPRINT_IMAGEFAIL: // 이미지 처리 실패
    Serial.println("Imaging error");
    return p;
  default: // 기타 오류 발생
    Serial.println("Unknown error");
    return p;
  }

  p = finger.image2Tz(); // 이미지를 특징으로 변환
  switch (p) {
  case FINGERPRINT_OK: // 변환 성공
    Serial.println("Image converted");
    break;
  case FINGERPRINT_IMAGEMESS: // 이미지가 너무 난잡함
    Serial.println("Image too messy");
    return p;
  case FINGERPRINT_PACKETRECIEVEERR: // 통신 오류 발생
    Serial.println("Communication error");
    return p;
  case FINGERPRINT_FEATUREFAIL: // 지문 특징을 찾을 수 없음
    Serial.println("Could not find fingerprint features");
    return p;
  case FINGERPRINT_INVALIDIMAGE: // 유효하지 않은 이미지
    Serial.println("Could not find fingerprint features");
    return p;
  default: // 기타 오류 발생
    Serial.println("Unknown error");
    return p;
  }

  p = finger.fingerFastSearch(); // 지문을 빠르게 검색
  if (p == FINGERPRINT_OK) { // 지문이 일치하는 경우
    Serial.println("Found a print match!");
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) { // 통신 오류 발생
    Serial.println("Communication error");
    return p;
  } 
  else if (p == FINGERPRINT_NOTFOUND) { // 일치하는 지문을 찾을 수 없음
    Serial.println("Did not find a match");
    return p;
  } 
  else { // 기타 오류 발생
    Serial.println("Unknown error");
    return p;
  }   

  Serial.print("Found ID #"); 
  Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); 
  Serial.println(finger.confidence); // 인식된 지문의 ID와 신뢰도를 시리얼 모니터에 출력

  return finger.fingerID; // 지문 ID 반환
}

int getFingerprintIDez() { // 간단한 지문 인식 및 ID 반환 함수 - 성공하면 1 반환 실패하면 -1 반환
  uint8_t p = finger.getImage(); // 지문 이미지를 캡처
  if (p != FINGERPRINT_OK)  return -1; // 실패하면 -1 반환

  p = finger.image2Tz(); // 이미지를 특징으로 변환
  if (p != FINGERPRINT_OK)  return -1; // 실패하면 -1 반환

  p = finger.fingerFastSearch(); // 지문을 빠르게 검색
  if (p != FINGERPRINT_OK)  return -1; // 실패하면 -1 반환

  Serial.print("Found ID #"); 
  Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); 
  Serial.println(finger.confidence); // 인식된 지문의 ID와 신뢰도를 시리얼 모니터에 출력
  return 1 ; // 성공 시 1 반환
}

uint8_t readnumber(void) { // 시리얼 모니터에서 숫자를 읽는 함수
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available()); // 시리얼 입력이 있을 때까지 대기
    num = Serial.parseInt(); // 입력된 숫자를 인식
  }
  return num; // 읽은 숫자를 반환
}

uint8_t getFingerprintEnroll() { // 지문 등록 함수
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); 
  Serial.println(id); // 등록된 지문을 기다리고 있음을 시리얼 모니터에 출력

  while (p != FINGERPRINT_OK) { // 지문 이미지가 캡처될 때까지 대기
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK: // 지문 이미지 캡처 성공
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER: // 지문이 감지되지 않음
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR: // 통신 오류 발생
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL: // 이미지 처리 실패
      Serial.println("Imaging error");
      break;
    default: // 기타 오류 발생
      Serial.println("Unknown error");
      break;
    }
  }

  p = finger.image2Tz(1); // 이미지를 템플릿으로 변환, 템플릿 ID는 1로 지정
  switch (p) {
  case FINGERPRINT_OK: // 변환 성공
    Serial.println("Image converted");
    break;
  case FINGERPRINT_IMAGEMESS: // 이미지가 너무 난잡함
    Serial.println("Image too messy");
    return p;
  case FINGERPRINT_PACKETRECIEVEERR: // 통신 오류 발생
    Serial.println("Communication error");
    return p;
  case FINGERPRINT_FEATUREFAIL: // 지문 특징을 찾을 수 없음
    Serial.println("Could not find fingerprint features");
    return p;
  case FINGERPRINT_INVALIDIMAGE: // 유효하지 않은 이미지
    Serial.println("Could not find fingerprint features");
    return p;
  default: // 기타 오류 발생
    Serial.println("Unknown error");
    return p;
  }

  Serial.println("Remove finger"); // 지문 제거 메시지 출력
  delay(2000); // 2000ms 대기
  p = 0;
  while (p != FINGERPRINT_NOFINGER) { // 지문이 완전히 제거될 때까지 대기
    p = finger.getImage();
  }
  Serial.print("ID "); 
  Serial.println(id); // 현재 ID를 시리얼 모니터에 출력
  p = -1;
  Serial.println("Place same finger again"); // 동일한 지문을 다시 요청하는 메시지 출력

  while (p != FINGERPRINT_OK) { // 지문 이미지가 캡처될 때까지 대기
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK: // 지문 이미지 캡처 성공
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER: // 지문이 감지되지 않음
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR: // 통신 오류 발생
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL: // 이미지 처리 실패
      Serial.println("Imaging error");
      break;
    default: // 기타 오류 발생
      Serial.println("Unknown error");
      break;
    }
  }

  p = finger.image2Tz(2); // 두 번째 템플릿으로 변환, 템플릿 ID는 2
  switch (p) {
  case FINGERPRINT_OK: // 변환 성공
    Serial.println("Image converted");
    break;
  case FINGERPRINT_IMAGEMESS: // 이미지가 너무 복잡함
    Serial.println("Image too messy");
    return p;
  case FINGERPRINT_PACKETRECIEVEERR: // 통신 오류 발생
    Serial.println("Communication error");
    return p;
  case FINGERPRINT_FEATUREFAIL: // 지문 특징을 찾을 수 없음
    Serial.println("Could not find fingerprint features");
    return p;
  case FINGERPRINT_INVALIDIMAGE: // 유효하지 않은 이미지
    Serial.println("Could not find fingerprint features");
    return p;
  default: // 기타 오류 발생
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("Creating model for #");  
  Serial.println(id); // 모델 생성 중임을 시리얼 모니터에 출력

  p = finger.createModel(); // 두 템플릿을 비교하여 모델 생성
  if (p == FINGERPRINT_OK) { // 모델 생성 성공
    Serial.println("Prints matched!");
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) { // 통신 오류 발생
    Serial.println("Communication error");
    return p;
  } 
  else if (p == FINGERPRINT_ENROLLMISMATCH) { // 두 템플릿이 일치하지 않음
    Serial.println("Fingerprints did not match");
    return p;
  } 
  else { // 기타 오류 발생
    Serial.println("Unknown error");
    return p;
  }   

  Serial.print("ID "); 
  Serial.println(id); // 현재 ID를 시리얼 모니터에 출력
  p = finger.storeModel(id); // 생성된 모델을 저장

  if (p == FINGERPRINT_OK) { // 저장 성공
    Serial.println("Stored!");
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) { // 통신 오류 발생
    Serial.println("Communication error");
    return p;
  } 
  else if (p == FINGERPRINT_BADLOCATION) { // 저장 위치 오류
    Serial.println("Could not store in that location");
    return p;
  } 
  else if (p == FINGERPRINT_FLASHERR) { // 플래시 메모리 쓰기 오류
    Serial.println("Error writing to flash");
    return p;
  } 
  else { // 기타 오류 발생
    Serial.println("Unknown error");
    return p;
  }   
}
