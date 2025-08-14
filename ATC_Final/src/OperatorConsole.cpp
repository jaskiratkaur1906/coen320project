#include "OperatorConsole.h"
using namespace std;

OperatorConsole::OperatorConsole() {}

OperatorConsole::~OperatorConsole() {}

void* OperatorConsole::startOperatorClient(void* context) {
    return ((OperatorConsole*)context)->operatorClient();
}

void* OperatorConsole::operatorClient() {
    int compServerCoid;
    data_t smsg, rmsg;
    int mode = 0;
    // open the log file for writing the operator log information
    writer.open("operator_log.txt");

    // computer system connection
    if ((compServerCoid = name_open(COMPUTER_SYSTEM_ATTACH, 0)) == -1) {
        perror("Error occurred while attaching the channel");
    }


    int n;
    int newVelocityX, newVelocityY, newVelocityZ;
    int newPosX, newPosY;
    int newAltitude;
    int aircraftId, commandType;

    string commandOptionMessage =
        "Choose Operation by entering number shown below: \n"
        "1: Request augmented information: \n"
        "2. Change Velocity \n"
        "3. Change Position \n"
        "4. Change Altitude \n"
        "5. Change PredictionTime \n"
        "Enter Operation: \n";


    while (true) {
            // Process user input and system responses based on the current mode.
    	switch (mode) {
    	    case 0: // Requesting aircraft ID from the user.
    	        cout << "Please input the aircraft ID: \n";
    	        cin >> aircraftId;

    	        if (cin.fail()) {
    	            cin.clear(); // Clear the error state of the input stream.
    	            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard bad input.
    	            cout << "Invalid input detected. Ensure you enter a numeric ID. \n";
    	        } else {
    	            smsg.hdr.type = static_cast<_Uint16t>(HEADER_TYPE::OPERATOR_VALID_ID);
    	            smsg.command_data.newAircraft.id = aircraftId;

    	            if (MsgSend(compServerCoid, &smsg, sizeof(smsg), &rmsg, sizeof(rmsg)) < 0) {
    	                cout << "Failed to send request due to an error. \n";
    	            } else {
    	                mode = !rmsg.command_data.validId ? 0 : 1; // Proceed based on aircraft ID validation.
    	                if (mode == 0) cout << "The provided aircraft ID is not valid. \n";
    	            }
    	        }
    	        break;

    	    case 1: // Selecting an operation.
    	        cout << commandOptionMessage;
    	        cin >> commandType;

    	        if (cin.fail()) {
    	            cin.clear();
    	            cin.ignore(numeric_limits<streamsize>::max(), '\n');
    	            cout << "Invalid command. Use numeric values to select an option.\n";
    	        } else {
    	            mode = (commandType >= 1 && commandType <= 5) ? commandType + 1 : 1;
    	            if (mode == 1) cout << "The command number is out of valid range.\n";
    	        }
    	        break;

    	    case 2: // Displaying augmented aircraft information.
    	        smsg.hdr.type = static_cast<_Uint16t>(HEADER_TYPE::OPERATOR_REQUEST_INFO);
    	        smsg.display_data.augmentedInfoAircraftId = aircraftId;

    	        if (MsgSend(compServerCoid, &smsg, sizeof(smsg), NULL, 0) < 0) {
    	            cout << "Request failed.\n";
    	        } else {
    	            logger(smsg); // Log the request.
    	            mode = 0; // Reset mode to initial state.
    	        }
    	        break;

    	    case 3: // Updating aircraft speed.
    	            cout << "Enter new speed for the x-direction: \n";
    	            cin >> newVelocityX;
    	            if (!cin.fail()) {
    	                cout << "Enter new speed for the y-direction: \n";
    	                cin >> newVelocityY;
    	                if (!cin.fail()) {
    	                    cout << "Enter new speed for the z-direction: \n";
    	                    cin >> newVelocityZ;
    	                    if (!cin.fail() && validSpeed(newVelocityX, newVelocityY, newVelocityZ)) {
    	                        smsg.hdr.type = static_cast<_Uint16t>(HEADER_TYPE::OPERATOR_COMMAND);
    	                        smsg.command_data.command = "CHANGE_SPEED";
    	                        smsg.command_data.newAircraft.speedX = newVelocityX;
    	                        smsg.command_data.newAircraft.speedY = newVelocityY;
    	                        smsg.command_data.newAircraft.speedZ = newVelocityZ;

    	                        if (MsgSend(compServerCoid, &smsg, sizeof(smsg), NULL, 0) < 0) {
    	                            cout << "Failed to update speed due to a communication error.\n";
    	                        } else {
    	                            logger(smsg); // Log this action.
    	                        }
    	                        mode = 0;
    	                    } else {
    	                        cout << "Invalid speed input. Please enter numeric values only.\n";
    	                        cin.clear();
    	                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    	                    }
    	                }
    	            }
    	            if (cin.fail()) {
    	                cin.clear();
    	                cin.ignore(numeric_limits<streamsize>::max(), '\n');
    	                cout << "Invalid speed input.\n";
    	                mode = 3;
    	            }
    	            break;

    	        case 4: // Updating aircraft position.
    	            cout << "Enter new x position: \n";
    	            cin >> newPosX;
    	            if (!cin.fail()) {
    	                cout << "Enter new y position: \n";
    	                cin >> newPosY;
    	                if (!cin.fail() && validPosition(newPosX, newPosY)) {
    	                    smsg.hdr.type = static_cast<_Uint16t>(HEADER_TYPE::OPERATOR_COMMAND);
    	                    smsg.command_data.command = "CHANGE_POSITION";
    	                    smsg.command_data.newAircraft.x = newPosX;
    	                    smsg.command_data.newAircraft.y = newPosY;

    	                    if (MsgSend(compServerCoid, &smsg, sizeof(smsg), NULL, 0) < 0) {
    	                        cout << "Failed to update position due to a communication error.\n";
    	                    } else {
    	                        logger(smsg); // Log the action.
    	                    }
    	                    mode = 0;
    	                } else {
    	                    cout << "Invalid position input. Please enter numeric values only.\n";
    	                    cin.clear();
    	                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    	                }
    	            }
    	            if (cin.fail()) {
    	                cin.clear();
    	                cin.ignore(numeric_limits<streamsize>::max(), '\n');
    	                cout << "Invalid position input.\n";
    	                mode = 4;
    	            }
    	            break;

    	        case 5: // Adjusting aircraft altitude.
    	            cout << "Enter new altitude: \n";
    	            cin >> newAltitude;
    	            if (!cin.fail() && validAltitude(newAltitude)) {
    	                smsg.hdr.type = static_cast<_Uint16t>(HEADER_TYPE::OPERATOR_COMMAND);
    	                smsg.command_data.command = "CHANGE_ALTITUDE";
    	                smsg.command_data.newAircraft.z = newAltitude;

    	                if (MsgSend(compServerCoid, &smsg, sizeof(smsg), NULL, 0) < 0) {
    	                    cout << "Failed to update altitude due to a communication error.\n";
    	                } else {
    	                    logger(smsg); // Log the action.
    	                }
    	                mode = 0;
    	            } else {
    	                cout << "Invalid altitude input. Please enter a numeric value.\n";
    	                cin.clear();
    	                cin.ignore(numeric_limits<streamsize>::max(), '\n');
    	                mode = 5;
    	            }
    	            break;

    	        case 6: // Setting new prediction time.
    	            cout << "Enter new prediction time in seconds: \n";
    	            cin >> n;
    	            if (!cin.fail() && n > 0) {
    	                smsg.hdr.type = static_cast<_Uint16t>(HEADER_TYPE::OPERATOR_PREDICTION_TIME);
    	                smsg.command_data.command = "CHANGE_PREDICTION_TIME";
    	                smsg.command_data.n = n;

    	                if (MsgSend(compServerCoid, &smsg, sizeof(smsg), NULL, 0) < 0) {
    	                    cout << "Failed to update prediction time due to a communication error.\n";
    	                } else {
    	                    logger(smsg); // Log the action.
    	                }
    	                mode = 0;
    	            } else {
    	                cout << "Invalid prediction time. Please enter a positive numeric value.\n";
    	                cin.clear();
    	                cin.ignore(numeric_limits<streamsize>::max(), '\n');
    	                mode = 6;
    	            }
    	            break;

    	        default:
    	            cout << "An unexpected error occurred. Returning to the main menu.\n";
    	            mode = 0;
    	            break;
    	    }
        }
    }

void OperatorConsole::logger(data_t msg) {
    // Determine the type of log message based on the header type
    switch (msg.hdr.type) {
        case static_cast<_Uint16t>(HEADER_TYPE::OPERATOR_REQUEST_INFO):
            writer << "Operator requested info for aircraft " << msg.display_data.augmentedInfoAircraftId << endl;
            break;
        case static_cast<_Uint16t>(HEADER_TYPE::OPERATOR_PREDICTION_TIME):
            writer << "Change prediction time to " << msg.command_data.n << " s" << endl;
            break;
        default:
            // Handle command-based logging
            aircraft_data_t& a = msg.command_data.newAircraft;
            if (msg.command_data.command == "CHANGE_SPEED") {
                writer << "Change aircraft " << a.id << " speed = (" << a.speedX << ", " << a.speedY << ", " << a.speedZ << ")" << endl;
            } else if (msg.command_data.command == "CHANGE_POSITION") {
                writer << "Change aircraft " << a.id << " pos = (" << a.x << ", " << a.y << ")" << endl;
            } else if (msg.command_data.command == "CHANGE_ALTITUDE") {
                writer << "Change aircraft " << a.id << " alt = (" << a.z << ")" << endl;
            }
            break;
    }
}


bool OperatorConsole::validSpeed(int newSpeedX, int newSpeedY, int newSpeedZ) {
    // Validates if the speed components are all positive.
    return newSpeedX > 0 && newSpeedY > 0 && newSpeedZ > 0;
}

bool OperatorConsole::validPosition(int x, int y) {
    // Checks if x and y positions are within a defined range.
    bool validX = x >= 0 && x <= 100000;
    bool validY = y >= 0 && y <= 100000;
    return validX && validY;
}

bool OperatorConsole::validAltitude(int z) {
    // Validates if altitude is within allowable bounds.
    return z >= 15000 && z <= 40000;
}
void OperatorConsole::startThreads() {
    //pthread_t operatorThreadId;

    // Create and start the operator client thread
    if (pthread_create(&operatorThreadId, NULL, startOperatorClient, this) != 0) {
        perror("Failed to create operator client thread");
    }

}
void OperatorConsole::joinThreads() {
    	int err_no = pthread_join(operatorThreadId, NULL);
    	if (err_no != 0){printf("ERROR when joining the thread\n");}
}
