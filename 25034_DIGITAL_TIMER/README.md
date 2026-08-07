# Digital Timer & Real-Time Clock Project

A C-based embedded application developed for the **PIC16F877A** microcontroller on the **PICGenios** board (supported in **PicSimLab**). The system functions as a digital clock and event timer using an external **DS1307 RTC** over **I2C**, displaying information on a 16x2 **Character LCD (CLCD)** and taking input from a **Digital Keypad**. When an event matches the RTC time, a **10-second buzzer alarm** is triggered automatically.

## 🌟 Features

* **Real-Time Clock (RTC):** Interfaces with a DS1307 RTC chip over I2C to display live Date (`DD-MM-YY`) and Time (`HH:MM AM/PM`).
* **Event Timer & Alarms:** Allows setting up to 5 individual events/alarms with custom times and repeat options (`Once`, `Daily`, `Weekly`).
* **Buzzer Alarm:** Triggers an automatic 10-second alarm via pin `RC2` when the RTC time matches a configured event.
* **Non-Blocking Logic:** Utilizes PIC Timer interrupts for smooth, non-blocking UI updates, button debouncing, and precise buzzer timing.
* **Configurable Time & Date:** Full menu navigation to adjust time, date, and saved events directly using the digital keypad.

## 🕹️ User Interface & Control Conventions

### **Button Navigation**
* **Short-Press UP (`RB1`):** Navigates menus / Increments blinking field values.
* **Short-Press DOWN (`RB2`):** Navigates menus / Advances to the next editable field.
* **Long-Press UP (`RB1`):** Selects menu item / Confirms and saves settings.
* **Long-Press DOWN (`RB2`):** Cancels editing / Exits to parent menu or default screen.

### **Display Modes**
1. **Default Clock Display:** Cycles between displaying current **Date** and **Time**, and next upcoming **Event**.
2. **Config Menu:** Toggle between `SET/VIEW EVENT` and `SET TIME/DATE`.
3. **Set Event Mode:** Configure Hour, Minute, AM/PM, Repeat type (`O`/`D`/`W`), and Duration.
4. **Set Time / Set Date Modes:** Interactively adjust RTC registers.
