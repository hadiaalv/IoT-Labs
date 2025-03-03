Task 2:

Without interrupt if we press button there will be no effect on screen

Task 3:

->What is a debounce issue, and why should we fix it?
When we press a button, it might seem like a simple action, but in reality, mechanical switches don’t settle immediately. Instead, they "bounce," creating multiple quick signals before stabilizing. This can lead to a single press being registered multiple times, causing unexpected behavior in your system.
To avoid false triggers and ensure reliable input detection, we apply debouncing—a technique to filter out these unwanted signals. This helps improve accuracy in applications where precise button presses matter.

->In which applications/domains can the debounce issue be threatening if not resolved?
Debounce problems can be more than just an annoyance—they can actually disrupt critical systems:

Embedded Systems (ESP32-S3, IoT, Home Automation): A bouncing signal can cause multiple unintended activations, leading to devices behaving unpredictably.
Industrial Control Systems: In machinery operations, an extra button press could trigger unintended movements, leading to malfunctions or even safety hazards.
Gaming Controllers: Imagine pressing a jump button, and your character jumps twice instead of once—frustrating, right? That’s a debounce issue!
Keypads & Data Entry Systems: If a single keypress is registered multiple times, it can lead to errors in PIN codes, passwords, or any critical input system.


->Why does debounce occur? Is it a compiler error, logical error, or a hardware issue?
Debounce occurs because of the mechanical nature of switches, not because of a programming bug or a compiler issue. When a button is pressed, its internal contacts physically vibrate before settling, sending multiple signals instead of just one.
Since this is a hardware issue, we fix it at the software level in MicroPython by:
Adding a delay (waiting a few milliseconds to let the signal stabilize).
Using interrupt-based handling with debounce logic to ensure only one input is registered per press.

Task 4:

->Why do we use Interrupt
Interrupts allow the ESP32-S3 to respond only when needed. This makes programs more efficient and responsive.
For example, interrupts are useful for:

Button presses – Detecting when a button is actually pressed without delay.
Sensors – Reacting instantly when new data is available.
Communication – Handling data from UART, SPI, or I2C without constantly checking.

->How does interrupt lower the processing cost of the micro-controller
Without interrupts, the microcontroller would waste time repeatedly checking for changes (called polling). This is like staring at your phone waiting for a message instead of doing something else and getting a notification when it arrives.

Interrupts help by:
Letting the microcontroller focus on other tasks instead of checking for events nonstop.
Responding only when needed, which saves power and processing time.
Making programs faster and more efficient, especially in real-time applications.