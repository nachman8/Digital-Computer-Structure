import serial as ser
import threading
import queue
import time


def main():
    # Shared state variables with thread locks
    shared_state = {
        'state3': False,
        'X': False,
        'i': 0,
        'o_Char': "",
        'x_input_complete': False
    }
    state_lock = threading.Lock()

    # Serial setup
    s = ser.Serial('COM3', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1)

    # Queues for thread communication
    tx_queue = queue.Queue()
    rx_data = queue.Queue()

    # Threading control
    stop_threads = threading.Event()

    menu = "\n                           Menu\n" \
           "1. Counting onto the LCD screen with a delay of X[ms]\n" \
           "2. Circular tone series via Buzzer with delay of X[ms]\n" \
           "3. Get delay time X[ms]:\n" \
           "4. Potentiometer 3-digit value [v] onto LCD\n" \
           "5. On each PB1 pressed, send a Massage \"I love my Negev\"\n" \
           "6. Clear LCD screen\n" \
           "7. Get a char a-z and show its ASCII code onto LCD\n" \
           "8. Show menu\n" \
           "9. Sleep\n"

    # Clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()

    def rx_thread():
        """RX Thread - Continuously reads from serial port"""
        while not stop_threads.is_set():
            try:
                if s.in_waiting > 0:
                    bytes_r_Char = s.read()
                    if bytes_r_Char:
                        r_char = bytes_r_Char.decode('ascii')
                        rx_data.put(r_char)
                else:
                    time.sleep(0.01)  # Small delay to prevent busy waiting
            except Exception as e:
                print(f"RX Error: {e}")
                time.sleep(0.1)

    def tx_thread():
        """TX Thread - Handles user input and sends data"""
        while not stop_threads.is_set():
            try:
                # Check if there's data in the queue to send first
                try:
                    data_to_send = tx_queue.get(timeout=0.1)
                    if data_to_send:
                        bytes_data = bytes(data_to_send, 'ascii')
                        s.write(bytes_data)
                        print(f"DEBUG: Sent '{data_to_send}' to MSP430")
                    tx_queue.task_done()
                    continue
                except queue.Empty:
                    pass

                # Handle user input when no data to send
                with state_lock:
                    current_state3 = shared_state['state3']
                    current_X = shared_state['X']
                    current_i = shared_state['i']
                    current_x_input_complete = shared_state['x_input_complete']

                if not current_state3:  # Normal menu selection
                    try:
                        # Check if input is needed
                        if not current_X and current_i == 0:  # Ready for new input
                            o_Char = input("Enter Menu Option: ")
                            while len(o_Char) != 1:
                                o_Char = input("Enter Menu Option: ")

                            with state_lock:
                                shared_state['o_Char'] = o_Char
                                if o_Char == '3':
                                    shared_state['state3'] = True
                                    shared_state['x_input_complete'] = False
                                    print("Starting X input mode...")

                            tx_queue.put(o_Char)

                    except KeyboardInterrupt:
                        stop_threads.set()
                        break

                else:  # State 3 - entering X value
                    if not current_X and not current_x_input_complete:
                        try:
                            X_input = input("Enter X Value between 1 to 1000 (ms): ")
                            # Validate X input
                            try:
                                x_val = int(X_input)
                                if x_val < 1 or x_val > 1000:
                                    print("Invalid X value. Must be between 1-1000.")
                                    continue
                            except ValueError:
                                print("Invalid X value. Must be a number.")
                                continue

                            X_input = X_input + '\n'

                            with state_lock:
                                shared_state['X'] = X_input
                                shared_state['i'] = 0

                            print(f"Sending X value: {X_input.strip()}")

                        except KeyboardInterrupt:
                            stop_threads.set()
                            break

                    # Send X characters one by one
                    with state_lock:
                        if shared_state['X'] and shared_state['i'] < len(shared_state['X']) and not shared_state[
                            'x_input_complete']:
                            char_to_send = shared_state['X'][shared_state['i']]
                            tx_queue.put(char_to_send)
                            print(f"Sending character '{char_to_send}' (index {shared_state['i']})")
                            shared_state['i'] += 1

                            # Check if we've sent all characters
                            if shared_state['i'] >= len(shared_state['X']):
                                shared_state['x_input_complete'] = True
                                print("X input complete, waiting for MSP430 response...")

                time.sleep(0.01)  # Small delay to prevent busy waiting

            except Exception as e:
                print(f"TX Error: {e}")
                time.sleep(0.1)

    # Start RX and TX threads
    rx_thread_obj = threading.Thread(target=rx_thread, daemon=True)
    tx_thread_obj = threading.Thread(target=tx_thread, daemon=True)

    rx_thread_obj.start()
    tx_thread_obj.start()

    print("RX Thread started")
    print("TX Thread started")
    print("Communication ready.")
    print(menu)

    try:
        while True:
            # Handle RX (receiving from MSP430)
            while not rx_data.empty():
                try:
                    r_char = rx_data.get_nowait()
                    print(f"DEBUG: Received '{r_char}' from MSP430")

                    with state_lock:
                        current_o_Char = shared_state['o_Char']
                        current_state3 = shared_state['state3']

                    if r_char == '5':
                        print("DEBUG PRINT - state5")
                        # Read the complete message for state 5
                        r_str = ""
                        timeout_counter = 0
                        message_started = False

                        while timeout_counter < 100:  # Timeout after 1 second
                            if not rx_data.empty():
                                char = rx_data.get_nowait()
                                if char == '\n':
                                    break
                                # Skip extra '5' characters at the beginning (acknowledgments)
                                if char == '5' and not message_started:
                                    continue
                                # Once we get a non-'5' character, message has started
                                if char != '5':
                                    message_started = True
                                r_str += char
                                timeout_counter = 0
                            else:
                                time.sleep(0.01)
                                timeout_counter += 1
                        print(r_str)

                    elif r_char == '7':
                        print("State 7: ASCII display mode active. Enter characters a-z to see ASCII codes.")

                    elif r_char == '8':
                        print(menu)
                        print("Option 8 done, state is now state0 - SLEEP")

                    elif r_char == 'R':
                        if current_o_Char == '3' and current_state3:
                            print("X value set successfully! MSP430 state is now state0 - SLEEP")
                            # Reset state 3 variables
                            with state_lock:
                                shared_state['state3'] = False
                                shared_state['X'] = False
                                shared_state['i'] = 0
                                shared_state['x_input_complete'] = False
                        elif current_o_Char != '3' and current_o_Char != '5' and current_o_Char != '8':
                            print(
                                "Option " + current_o_Char + " is running, MSP430 state is now state" + current_o_Char)
                        else:
                            print("Option " + current_o_Char + " done, MSP430 state is now state0 - SLEEP")
                            with state_lock:
                                shared_state['state3'] = False
                                shared_state['X'] = False
                                shared_state['i'] = 0
                                shared_state['x_input_complete'] = False

                    elif r_char == 'E':
                        print("MSP430 received invalid Char, MSP430 state is now state0 - SLEEP")
                        # Reset all state variables
                        with state_lock:
                            shared_state['state3'] = False
                            shared_state['X'] = False
                            shared_state['i'] = 0
                            shared_state['x_input_complete'] = False

                    elif r_char == '3':
                        if current_state3:
                            print("MSP430 ready for next digit")
                            # Don't reset enableTX here - let it be handled below
                        else:
                            print("Received unexpected '3' response")

                    else:
                        print(r_char, end='')
                        if not current_state3:
                            # Reset state variables for non-state3 operations
                            with state_lock:
                                shared_state['X'] = False
                                shared_state['i'] = 0
                                shared_state['x_input_complete'] = False

                except queue.Empty:
                    break

            # Small delay to prevent busy waiting
            time.sleep(0.01)

    except KeyboardInterrupt:
        print("\nShutting down...")
        stop_threads.set()

        # Wait for threads to finish
        rx_thread_obj.join(timeout=1)
        tx_thread_obj.join(timeout=1)

        # Close serial connection
        s.close()


if __name__ == '__main__':
    main()