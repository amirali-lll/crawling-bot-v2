# راهنمای مهاجرت: از Arduino به PlatformIO

## خلاصه تغییرات

این سند تفاوت‌های کلیدی بین قالب قدیمی Arduino و پیاده‌سازی جدید PlatformIO را شرح می‌دهد.

## تغییرات سخت‌افزاری

| قطعه         | قدیمی (Arduino)   | جدید (PlatformIO)          |
| ------------ | ----------------- | -------------------------- |
| نمایشگر      | LCD 16x2 (I2C)    | OLED 0.96" SSD1306         |
| سنسور فاصله  | آلتراسونیک SRF-05 | MPU9250 AHRS (IMU 9 محوری) |
| برد          | ESP32 Dev Module  | ESP32-S3-DevKitM-1         |
| سروو موتورها | 2x موتور سروو     | 2x موتور سروو (یکسان)      |

## تغییرات معماری کد

### ساختار قدیمی (یکپارچه)

```
old-template.ino
└── تمام کد در یک فایل
    ├── متغیرهای سراسری
    ├── توابع کمکی
    ├── setup()
    └── loop()
```

### ساختار جدید (مبتنی بر کتابخانه)

```
src/main.cpp              # برنامه اصلی
lib/
├── Display/              # مدیریت نمایشگر OLED
├── AHRS/                 # سنسور MPU9250 و ردیابی حرکت
├── ServoControl/         # کنترل موتور سروو
├── Network/              # WiFi AP و بروزرسانی OTA
└── Training/             # آموزش RL (قالب)
```

## تغییرات API

### نمایشگر

**قدیمی (LCD):**

```cpp
LiquidCrystal_I2C lcd(0x27, 16, 2);
lcd.init();
lcd.backlight();
lcd.clear();
lcd.print("Hello");
lcd.setCursor(0, 1);
lcd.print("World");
```

**جدید (OLED):**

```cpp
Display display;
display.begin();
display.clear();
display.print("Hello", 0, 0);
display.print("World", 0, 16);
```

### تشخیص فاصله/حرکت

**قدیمی (SRF-05):**

```cpp
NewPing sonar(triggerPin, echoPin, MAX_DISTANCE);
float distance = sonar.ping_cm();
if (distance == 0) {
    // هیچ جسمی شناسایی نشد
}
```

**جدید (MPU9250 AHRS):**

```cpp
AHRS ahrs;
ahrs.begin();
ahrs.update();  // در loop فراخوانی شود

// تشخیص حرکت
bool moving = ahrs.isMoving();

// جهت‌گیری
float roll = ahrs.getRoll();
float pitch = ahrs.getPitch();
float yaw = ahrs.getYaw();

// ردیابی جابجایی
float dx = ahrs.getDisplacementX();
float dy = ahrs.getDisplacementY();

// داده‌های خام سنسور
float accelX = ahrs.getAccelX();
float gyroZ = ahrs.getGyroZ();
float magX = ahrs.getMagX();
```

### کنترل سروو

**قدیمی:**

```cpp
Servo servodown;
Servo servoup;
servodown.attach(control_servo_down, 600, 2400);
servoup.attach(control_servo_up, 600, 2400);
servodown.write(90);

// حرکت نرم
moveServoSmooth(servodown, 0, 90, 10);
```

**جدید:**

```cpp
ServoControl servos(SERVO_PIN_DOWN, SERVO_PIN_UP);
servos.begin();
servos.moveDown(90);

// حرکت نرم
servos.moveDownSmooth(90, 10);
servos.setInitialPosition();
```

### شبکه و OTA

**قدیمی:**

```cpp
// توابع جداگانه
setup_ap();
setup_ota();

// ایجاد دستی task
xTaskCreatePinnedToCore(otaTask, "OTATask", ...);
```

**جدید:**

```cpp
Network network(&display);
network.begin();          // راه‌اندازی AP و OTA
network.startOTATask();   // شروع task در FreeRTOS
uint8_t num = network.getRobotNumber();
```

## تعریف پین‌ها

**قدیمی:**

```cpp
const int triggerPin = 5;        // trigger سنسور SRF-05
const int echoPin = 17;          // echo سنسور SRF-05
const int control_servo_down = 32;
const int control_servo_up = 33;
```

**جدید:**

```cpp
// config.h
#define SERVO_PIN_DOWN 32
#define SERVO_PIN_UP 33
// پین‌های I2C مقادیر پیش‌فرض هستند (SDA: 21, SCL: 22)
// نیازی به پین‌های سنسور آلتراسونیک نیست
```

## بهبودهای قابلیت‌ها

### 1. قابلیت‌های بهبود یافته حسگری

MPU9250 داده‌های بسیار بیشتری نسبت به SRF-05 ارائه می‌دهد:

**قابلیت‌های قدیمی:**

- فقط اندازه‌گیری فاصله
- محدود به دید مستقیم

**قابلیت‌های جدید:**

- حسگری حرکت 9 محوری (شتاب‌سنج، ژیروسکوپ، مگنتومتر)
- جهت‌گیری (roll, pitch, yaw)
- نمایش quaternion
- ردیابی جابجایی
- تشخیص حرکت
- سنجش دما
- نیازی به دید مستقیم نیست

### 2. نمایشگر بهتر

**قدیمی (LCD 16x2):**

- 16 کاراکتر × 2 خط
- فقط متن
- کنترل نور پس‌زمینه

**جدید (OLED 0.96"):**

- 128×64 پیکسل
- قابلیت نمایش گرافیک
- نوار پیشرفت
- کنتراست بهتر
- تراکم اطلاعات بیشتر

### 3. معماری ماژولار

**مزایا:**

- آزمایش آسان‌تر اجزای مجزا
- سازماندهی بهتر کد
- کتابخانه‌های قابل استفاده مجدد
- نگهداری ساده‌تر
- جداسازی واضح مسئولیت‌ها

### 4. چارچوب آموزش

**قدیمی:**

```cpp
void doTraining() {
    // TODO
}
```

**جدید:**

```cpp
Training training;
training.begin();
training.startTraining();
training.executeLearnedBehavior();
// رابط کاربری مشخص برای پیاده‌سازی آینده RL
```

## مراحل مهاجرت برای توسعه‌دهندگان

1. **به‌روزرسانی platformio.ini** با وابستگی‌های جدید
2. **ایجاد ساختار کتابخانه** در دایرکتوری `lib/`
3. **انتقال کد نمایشگر** از API LCD به OLED
4. **جایگزینی تشخیص فاصله** با ردیابی حرکت AHRS
5. **پوشش‌دهی کنترل سروو** در کلاس اختصاصی
6. **کپسوله‌سازی شبکه** در کلاس Network
7. **به‌روزرسانی main.cpp** برای استفاده از کتابخانه‌های جدید
8. **آزمایش هر ماژول** به صورت مستقل
9. **پیاده‌سازی آموزش** زمانی که آماده شد (در حال حاضر placeholder)

## نکات سازگاری معکوس

این یک **تغییر breaking** است - کد قدیمی بدون اصلاحات روی سخت‌افزار جدید اجرا نخواهد شد زیرا:

1. درایور نمایشگر متفاوت (LCD → OLED)
2. سنسور متفاوت (آلتراسونیک → IMU)
3. وابستگی‌های کتابخانه متفاوت
4. معماری متفاوت

با این حال، تمام **قابلیت‌ها** حفظ یا بهبود یافته‌اند:

- ✅ خروجی نمایشگر
- ✅ تشخیص حرکت/فاصله (بهبود یافته)
- ✅ کنترل سروو
- ✅ WiFi AP
- ✅ بروزرسانی‌های OTA
- ✅ پیکربندی شماره ربات
- ✅ بررسی سلامت
- ✅ چارچوب آموزش (آماده برای پیاده‌سازی)

## تغییرات فرآیند ساخت

**قدیمی:**

- محیط Arduino IDE
- نصب دستی کتابخانه
- انتخاب برد در IDE
- آپلود از طریق USB

**جدید:**

- PlatformIO (خط فرمان یا IDE)
- مدیریت خودکار وابستگی‌ها
- برد پیکربندی شده در platformio.ini
- آپلود از طریق USB یا OTA

## مراحل بعدی

بعد از مهاجرت:

1. آزمایش تمام اجزای سخت‌افزاری
2. کالیبره کردن MPU9250 برای خوانش‌های دقیق
3. پیاده‌سازی الگوریتم‌های آموزش در کتابخانه Training
4. تنظیم دقیق حرکات سروو برای مکانیک ربات شما
5. سفارشی‌سازی خروجی نمایشگر برای مورد استفاده شما

## سوالی دارید؟

به این‌ها مراجعه کنید:

- `README.md` - مستندات کلی پروژه
- `BUILD.md` - دستورالعمل‌های ساخت و استقرار
- فایل‌های منبع کتابخانه برای جزئیات API
