#include <iostream>
using namespace std;
#include <Windows.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <conio.h>

struct InstructionType;
struct ReservationStation;
struct Instruction;
struct LoadBuffer;
struct StoreBuffer;
struct InstructionStatus;
struct Tomasulo;


/*Instruction Status has the information about when an instruction was issued, started its execution, finished its execution,
and wrote back.*/
struct InstructionStatus
{
	
	short  issue;
	short executionStart;
	short executionComplete;
	short writeBack;

	short executionCyclesRemaining;
	InstructionStatus()
	{
		issue = executionStart = executionComplete = writeBack = executionCyclesRemaining= -1;
	}
};

struct InstructionType
{
public:
	string static const MULTD;
	string static const SUBD;
	string static const ADDD;
	string static const DIVD;
	string static const LOAD;
	string static const BNE;
	string static const STORE;
};

string const InstructionType::MULTD = "MUL";
string const InstructionType::SUBD = "SUB";
string const InstructionType::ADDD = "ADD";
string const InstructionType::DIVD = "DIV";

string const InstructionType::LOAD="LOAD";
string const InstructionType::BNE="BNE";
string const InstructionType::STORE = "STORE";

struct ReservationStationType
{
	static string const ADDD;
	static string const MULTD;
};

string const ReservationStationType::ADDD = "ADD";
string const ReservationStationType::MULTD = "MUL";

struct LoadStoreBufferType
{
	static string const LOAD;
	static string const STORE;
};

string const LoadStoreBufferType::LOAD = "LOAD";
string const LoadStoreBufferType::STORE = "STORE";


struct Instruction
{
	string instructionType;
	string Rd;
	string Rs;
	string Rt;
	int immediateOffset; //used only for i-type instructions
	InstructionStatus instructionStatus;

 	Instruction()
	{
		instructionType = Rd = Rs = Rt = "";
	immediateOffset = -1;
	}
};

struct RegisterStatus
{
	string registerName;
	string writingUnit;

};

struct ReservationStation
{
	string name;
	bool isBusy;
	string instructionType;
	string Vj;
	string Vk;
	string Qj;
	string Qk;
	
	Instruction *instruction; //the pointer to the current instruction being executed

	ReservationStation()
	{
		isBusy = false;
	instruction = nullptr;
	}


};

struct LoadStoreBuffer
{   
	string name;
	bool isBusy;
	string address;

	LoadStoreBuffer()
	{
		name = "";
		isBusy = false;
		address = "";
		instruction = nullptr;
		fu = "";
	}

	string fu;
	Instruction *instruction;
};

void gotoxy(short, short);

struct Tomasulo
{

	string reason;
	int currentCycleNumber;
	LoadStoreBuffer *loadBuffers;
	LoadStoreBuffer *storeBuffers;
	ReservationStation *addSubReservationStations;
	ReservationStation *multDivReservationStations;
	RegisterStatus *registerStatus;


	int totalLoadBuffers;
	int totalStoreBuffers;
	int totalAddSubReservationStations;
	int totalMultDivReservationStations;
	
	int totalRegisters;
	Instruction* instructions;
	int totalInstructions;
   
	//the following fields tell us how many cycles does each instruction type take in execution stage.
	int totalLoadStoreCycles;
	int totalAddSubCycles;
	int totalMultCylcles;
	int totalDivCycles;


	//load instructions and other information from the file.
	void loadDataFromFile(string fileName)
	{

		if (fileName == "")
		{
			fileName = "source.txt";
		}

		ifstream read;
		read.open(fileName);
		if (read.is_open() == false)
		{
			cout << "File could not open!" << endl;
			system("pause");
			exit(EXIT_FAILURE);
		}

		string data;
		while (read.peek() != '#')
			read.ignore();

		getline(read, data); //ignore the first line;
		//cout << data << endl;
		bool loadDone = false,
			storeDone = false,
			addSubDone = false,
			multDivDone = false;

		bool exit_ = false;

		//read information about the number of buffers/ reservation stations
		for (int i = 0; i < 4; i++)
		{
			read >> data;
		//	cout << data << " ";
			if (_strcmpi(data.c_str(), "Add_Sub_Reservation_Stations") == 0)
			{
				addSubDone = true;
				read >> this->totalAddSubReservationStations;
				//cout << this->totalAddSubReservationStations << endl;
			}

			else if (_strcmpi(data.c_str(), "Mul_Div_Reservation_Stations") == 0)
			{
				multDivDone = true;
				read >> this->totalMultDivReservationStations;
			//	cout << this->totalMultDivReservationStations << endl;
			}

			else if (_strcmpi(data.c_str(), "Load_Buffers") == 0)
			{
				loadDone = true;
				read >> this->totalLoadBuffers;
			//	cout << this->totalLoadBuffers << endl;
			}

			else if (_strcmpi(data.c_str(), "Store_Buffers") == 0)
			{
				storeDone = true;
				read >> this->totalStoreBuffers;
			//	cout << this->totalStoreBuffers << endl;
			}

		}
			if (!storeDone)
			{
				cout << "-> Information about the number of store buffers missing." << endl;
				exit_ = true;
		
			}
			else if (!loadDone)
			{
				cout << "-> Informaton about the number of load buffers missing." << endl;
				exit_ = true;
			}
			else if (!multDivDone)
			{
				cout << "-> Information about the number of Mult Div Reservation Stations missing" << endl;
				exit_ = true;
			}

			else if (!addSubDone)
			{
				cout << "-> Information about the number of Add Sub Reservation Stations missing." << endl;
				exit_ = true;
			}
		


		//reading information about cycle instructions
			bool multCyclesDone = false,
				 divCyclesDones = false,
				 addSubCyclesDone = false,
				 loadStoreCyclesDone = false;
		
			while (read.peek() != '#')
				read.ignore();

		getline(read,data); //ignore the line containing comments
		//cout << data << endl;
		for (int i = 0; i < 4; i++)
		{
			read >> data;
			//cout << data << " ";
			if (_strcmpi(data.c_str(), "Add_Sub_Cycles") == 0)
			{

				read >> this->totalAddSubCycles;
			//	cout << this->totalAddSubCycles << endl;
				addSubCyclesDone = true;
			}

			else if (_strcmpi(data.c_str(), "Mul_Cycles") == 0)
			{
				read >> this->totalMultCylcles;
			//	cout << this->totalMultCylcles << endl;
				multCyclesDone = true;
			}
			else if (_strcmpi(data.c_str(), "Load_Store_Cycles") == 0)
			{
				read >> this->totalLoadStoreCycles;
			//	cout << this->totalLoadStoreCycles << endl;
				loadStoreCyclesDone = true;
			}
			else if (_strcmpi(data.c_str(), "Div_Cycles") == 0)
			{
				read >> this->totalDivCycles;
			//	cout << this->totalDivCycles << endl;
				divCyclesDones = true;
			}

		}

				if (!addSubCyclesDone)
				{
					cout << "-> Information about the number of add Sub Cycles missing.";
					exit_ = true;
				}

				if (!multCyclesDone)
				{
					cout << "-> Information about the number of mul cycles missing." << endl;
					exit_ = true;
				}
				else if (!divCyclesDones)
				{
					cout << "-> Information about the number of div cycles missing." << endl;
					exit_ = true;
				}
				else if (!loadStoreCyclesDone)
				{
					cout << "-> Information about the number of load store cycles missing." << endl;
					exit_ = true;
				}

				if (exit_ == true)
				{
					system("pause");
					exit(EXIT_FAILURE);
					
				}

				this->loadBuffers = new LoadStoreBuffer[this->totalLoadBuffers];
				for (int i = 0; i < totalLoadBuffers; i++)
				{
					char num[10];
					_itoa_s(i, num, 10);
					this->loadBuffers[i].name = LoadStoreBufferType::LOAD;
					this->loadBuffers[i].name.append(num);
				}

				this->storeBuffers = new LoadStoreBuffer[this->totalStoreBuffers];
				for (int i = 0; i < totalStoreBuffers; i++)
				{
					char num[10];
					_itoa_s(i, num, 10);
			//		cout << "NUM: " << num << endl;
			//		system("pause");
					this->storeBuffers[i].name = LoadStoreBufferType::STORE;
					this->storeBuffers[i].name.append(num);
					//cout << this->storeBuffers[i].name;
				}

				this->addSubReservationStations = new ReservationStation[this->totalAddSubReservationStations];
				for (int i = 0; i < this->totalAddSubReservationStations; i++)
				{
					char num[10];
					_itoa_s(i, num, 10);
					this->addSubReservationStations[i].name = ReservationStationType::ADDD;
					this->addSubReservationStations[i].name.append(num);
				}

				this->multDivReservationStations = new ReservationStation[this->totalMultDivReservationStations];
				for (int i = 0; i < this->totalMultDivReservationStations; i++)
				{
					char num[10];
					_itoa_s(i, num, 10);
					this->multDivReservationStations[i].name = ReservationStationType::MULTD;
					this->multDivReservationStations[i].name.append(num);
				}



				while (read.peek() != '#')
					read.ignore();

				//reading information about the number of registers;
				getline(read, data);
				read >> data;
				read >> this->totalRegisters;
				this->registerStatus = new RegisterStatus[totalRegisters];
				
				for (int i = 0; i < totalRegisters; i++)
				{
					this->registerStatus[i].registerName = "F";
					char num[10];
					_itoa_s(i, num, 10);
					this->registerStatus[i].registerName.append(num);
				}


				//reading instructions
				while (read.peek() != '#')
					read.ignore();

				getline(read, data); //ignore the first line
				read >> this->totalInstructions;
				this->instructions = new Instruction[this->totalInstructions];

				for (int i = 0; i < totalInstructions; i++)
				{
					string instruction;
					read >> instruction;
					if (_strcmpi(instruction.c_str(), InstructionType::ADDD.c_str()) == 0 )
					{
						this->instructions[i].instructionType = InstructionType::ADDD;
						read >> this->instructions[i].Rd;
						read >> this->instructions[i].Rs;
						read >> this->instructions[i].Rt;
					}

					else if (_strcmpi(instruction.c_str(), InstructionType::SUBD.c_str()) == 0)
					{
						this->instructions[i].instructionType = InstructionType::SUBD;
						read >> this->instructions[i].Rd;
						read >> this->instructions[i].Rs;
						read >> this->instructions[i].Rt;

					}
					else if (_strcmpi(instruction.c_str(), InstructionType::MULTD.c_str()) == 0)
					{
						this->instructions[i].instructionType = InstructionType::MULTD;
						read >> this->instructions[i].Rd;
						read >> this->instructions[i].Rs;
						read >> this->instructions[i].Rt;

					}
					else if (_strcmpi(instruction.c_str(), InstructionType::DIVD.c_str()) == 0)
					{
						this->instructions[i].instructionType = InstructionType::DIVD;
						read >> this->instructions[i].Rd;
						read >> this->instructions[i].Rs;
						read >> this->instructions[i].Rt;

					}

					else if (_strcmpi(instruction.c_str(), InstructionType::LOAD.c_str())==0)
					{
						this->instructions[i].instructionType = InstructionType::LOAD;
						read >> this->instructions[i].Rt;
						read >> this->instructions[i].immediateOffset;
						read >> this->instructions[i].Rs;
					}

					else if (_strcmpi(instruction.c_str(), InstructionType::STORE.c_str()) == 0)
					{
						this->instructions[i].instructionType = InstructionType::STORE;
						read >> this->instructions[i].Rt;
						read >> this->instructions[i].immediateOffset;
						read >> this->instructions[i].Rs;

					}


				
				}
				
			

	}
	int findFreeReservationStationForStoreInstruction()
	{
		for (int i = 0; i < totalStoreBuffers; i++)
		{
			if (storeBuffers[i].isBusy == false)
				return i;
		}

		return -1;
	}

	int findFreeReservationStationforAddSubInstruction()
	{
		for (int i = 0; i < totalAddSubReservationStations; i++)
		{
			if (addSubReservationStations[i].isBusy == false)
			{
				return i;
			}
		}
		return -1;
	}

	int findFreeReservationStationforMultDivInstruction()
	{
		for (int i = 0; i < totalMultDivReservationStations; i++)
		{
			if (multDivReservationStations[i].isBusy == false)
			{
				return i;
			}
		}
		return -1;
	}
	int findFreeRservationStationForLoadInstruction()
	{
		for (int i = 0; i <totalLoadBuffers; i++)
		{
			if (loadBuffers[i].isBusy == false)
			return i;

	    }

		return -1;

	}

	void broadCastResult(string val, string name)
	{
		for (int i = 0; i < totalLoadBuffers; i++)
		{
			if (this->loadBuffers[i].isBusy == false)
				continue;

			if (this->loadBuffers[i].fu == name)
			{
				this->loadBuffers[i].fu = "";
			}
		}

		for (int i = 0; i < totalStoreBuffers; i++)
		{
			if (this->storeBuffers[i].isBusy == false)
				continue;

			if (this->storeBuffers[i].fu == name)
			{
				this->storeBuffers[i].fu = "";
			}
		}

		for (int i = 0; i < totalAddSubReservationStations; i++)
		{
			if (this->addSubReservationStations[i].Qj == name)
			{
				this->addSubReservationStations[i].Qj = "";
				this->addSubReservationStations[i].Vj = val;

			}

			if (this->addSubReservationStations[i].Qk == name)
			{
				this->addSubReservationStations[i].Qk = "";
				this->addSubReservationStations[i].Vk = val;
			}
		}

		for (int i = 0; i < totalMultDivReservationStations; i++)
		{
			if (this->multDivReservationStations[i].Qj == name)
			{
				this->multDivReservationStations[i].Qj = "";
				this->multDivReservationStations[i].Vj = val;

			}

			if (this->multDivReservationStations[i].Qk == name)
			{
				this->multDivReservationStations[i].Qk = "";
				this->multDivReservationStations[i].Vk = val;
			}
		}
	}

	
	int issueInstruction(int currentInstructionNumberToBeIssued)
	{
		int bufferNo;
		char num[10];
		_itoa_s(currentInstructionNumberToBeIssued, num, 10);

		if (currentInstructionNumberToBeIssued < totalInstructions)
		{
			if (instructions[currentInstructionNumberToBeIssued].instructionType == InstructionType::LOAD)
			{
				bufferNo = findFreeRservationStationForLoadInstruction();

				if (bufferNo == -1)
				{   
			
					reason += "-> The instruction Number: ";
					reason.append(num);
					reason+=" has not been issued due to the structural hazard.\n";

					return -1;
				}
				else
				{
					loadBuffers[bufferNo].isBusy = true;
					loadBuffers[bufferNo].instruction = &this->instructions[currentInstructionNumberToBeIssued];
					loadBuffers[bufferNo].address = this->instructions[currentInstructionNumberToBeIssued].Rs;
					char offset[10];
					_itoa_s(this->instructions[currentInstructionNumberToBeIssued].immediateOffset, offset, 10);
					loadBuffers[bufferNo].address += " + ";
					loadBuffers[bufferNo].address.append(offset);
					this->instructions[currentInstructionNumberToBeIssued].instructionStatus.issue = currentCycleNumber;
					reason += "-> The instruction number: ";
					reason.append(num);
					reason += " has been issued at Load Buffer= "+loadBuffers[bufferNo].name+"\n";

					this->instructions[currentInstructionNumberToBeIssued].instructionStatus.executionCyclesRemaining = this->totalLoadStoreCycles;
					//RAW hazard check
					int regNumber = atoi(&this->instructions[currentInstructionNumberToBeIssued].Rt.c_str()[1]);//extract register number fro register name
					this->loadBuffers[bufferNo].fu = this->registerStatus[regNumber].writingUnit;
					
				 
					//set the register status of register which is going to be written by this load instruction
					this->registerStatus[regNumber].writingUnit = loadBuffers[bufferNo].name; 

				

				    
				}
			
            }


			else if (instructions[currentInstructionNumberToBeIssued].instructionType == InstructionType::STORE)
			{
				 bufferNo = findFreeReservationStationForStoreInstruction();
				
				if (bufferNo == -1)
				{
					reason += "-> The instruction number: ";
					reason.append(num);
					reason += " has not been issued due to the structural hazard.\n";
					return -1;
				}
				else
				{

					reason += "-> The instruction number: ";
					reason.append(num);
					reason += " has been issued at Store Buffer= " + storeBuffers[bufferNo].name + "\n";

					this->storeBuffers[bufferNo].isBusy = true;
					
					//checking RAW hazard
					int regNumber = atoi(&this->instructions[currentInstructionNumberToBeIssued].Rt.c_str()[1]);
					this->storeBuffers[bufferNo].fu = this->registerStatus[regNumber].writingUnit;
				
					this->instructions[currentInstructionNumberToBeIssued].instructionStatus.executionCyclesRemaining=this->totalLoadStoreCycles;

					storeBuffers[bufferNo].isBusy = true;
					storeBuffers[bufferNo].instruction = &this->instructions[currentInstructionNumberToBeIssued];
					this->instructions[currentInstructionNumberToBeIssued].instructionStatus.issue = currentCycleNumber;
					storeBuffers[bufferNo].address = this->instructions[currentInstructionNumberToBeIssued].Rs;
					char offset[10];
					_itoa_s(this->instructions[currentInstructionNumberToBeIssued].immediateOffset, offset, 10);
					storeBuffers[bufferNo].address += " + ";
					storeBuffers[bufferNo].address.append(offset);

					

				
				}
			}

			else if (instructions[currentInstructionNumberToBeIssued].instructionType == InstructionType::ADDD || instructions[currentInstructionNumberToBeIssued].instructionType == InstructionType::SUBD)
			{
				int bufferNo = findFreeReservationStationforAddSubInstruction();
				

				if (bufferNo == -1)
				{

					reason += "-> The instruction No: ";
					reason.append(num);
					reason += " cannot be issued due to structural Hazard!\n";
					return -1;
				}

				else
				{

					reason += "-> The instruction No: ";
					reason.append(num);
					reason += " has been issued at Reservation Station= "+addSubReservationStations[bufferNo].name+ "\n";

					
				
					this->instructions[currentInstructionNumberToBeIssued].instructionStatus.executionCyclesRemaining = this->totalAddSubCycles;
					this->addSubReservationStations[bufferNo].instructionType = this->instructions[currentInstructionNumberToBeIssued].instructionType;
					this->addSubReservationStations[bufferNo].instruction = &this->instructions[currentInstructionNumberToBeIssued];
					this->addSubReservationStations[bufferNo].isBusy = true;
					this->instructions[currentInstructionNumberToBeIssued].instructionStatus.issue = currentCycleNumber;
				
				
					//checking RAW hazard 
					//RS
					int regNumber = atoi(&this->instructions[currentInstructionNumberToBeIssued].Rs.c_str()[1]);
				
					this->addSubReservationStations[bufferNo].Qj = this->registerStatus[regNumber].writingUnit;
				
					//Rt
					regNumber = atoi(&this->instructions[currentInstructionNumberToBeIssued].Rt.c_str()[1]);
					this->addSubReservationStations[bufferNo].Qk = this->registerStatus[regNumber].writingUnit;

					if (this->addSubReservationStations[bufferNo].Qj == "")
					{
						this->addSubReservationStations[bufferNo].Vj = "R(" + this->instructions[currentInstructionNumberToBeIssued].Rs + ")";
					}

					if (this->addSubReservationStations[bufferNo].Qk == "")
					{
						this->addSubReservationStations[bufferNo].Vk = "R(" + this->instructions[currentInstructionNumberToBeIssued].Rt + ")";
					}


					//setting register status
					regNumber = atoi(&this->instructions[currentInstructionNumberToBeIssued].Rd.c_str()[1]);
					this->registerStatus[regNumber].writingUnit = this->addSubReservationStations[bufferNo].name;

	
				}
			}

			else if (instructions[currentInstructionNumberToBeIssued].instructionType == InstructionType::MULTD || instructions[currentInstructionNumberToBeIssued].instructionType == InstructionType::DIVD)
			{
				int bufferNo = findFreeReservationStationforMultDivInstruction();
				char instructionNumberConversion[10];
				_itoa_s(currentInstructionNumberToBeIssued, instructionNumberConversion, 10, 10);

				if (bufferNo == -1)
				{

					reason += "-> The instruction No: ";
					reason.append(instructionNumberConversion);
					reason += " cannot be issued due to structural Hazard!\n";
					return -1;
				}

				else
				{

					reason += "-> The instruction No: ";
					reason.append(num);
					reason += " has been issued at Reservation Station= " + this->multDivReservationStations[bufferNo].name + "\n";


					if (this->instructions[currentInstructionNumberToBeIssued].instructionType == InstructionType::MULTD)
						this->instructions[currentInstructionNumberToBeIssued].instructionStatus.executionCyclesRemaining = this->totalMultCylcles;
					else
						this->instructions[currentInstructionNumberToBeIssued].instructionStatus.executionCyclesRemaining = this->totalDivCycles;
				
					this->multDivReservationStations[bufferNo].instructionType = this->instructions[currentInstructionNumberToBeIssued].instructionType;
					this->multDivReservationStations[bufferNo].instruction = &this->instructions[currentInstructionNumberToBeIssued];
					this->multDivReservationStations[bufferNo].isBusy = true;
					this->instructions[currentInstructionNumberToBeIssued].instructionStatus.issue = currentCycleNumber;


					//checking RAW hazard 
					//RS
					int regNumber = atoi(&this->instructions[currentInstructionNumberToBeIssued].Rs.c_str()[1]);
					
					this->multDivReservationStations[bufferNo].Qj = this->registerStatus[regNumber].writingUnit;

					//Rt
					regNumber = atoi(&this->instructions[currentInstructionNumberToBeIssued].Rt.c_str()[1]);
				
					this->multDivReservationStations[bufferNo].Qk = this->registerStatus[regNumber].writingUnit;

					if (this->multDivReservationStations[bufferNo].Qj == "")
					{  
						this->multDivReservationStations[bufferNo].Vj = "R(" + this->instructions[currentInstructionNumberToBeIssued].Rs + ")";
					}

					if (this->multDivReservationStations[bufferNo].Qk == "")
					{
						
					
						this->multDivReservationStations[bufferNo].Vk = "R(" + this->instructions[currentInstructionNumberToBeIssued].Rt + ")";
						
					
					}

					//setting register status
					regNumber = atoi(&this->instructions[currentInstructionNumberToBeIssued].Rd.c_str()[1]);
				
					this->registerStatus[regNumber].writingUnit = this->multDivReservationStations[bufferNo].name;

				}
			}


		}
		return 0;
	}

	void writeBack()
	{
	

		for (int i = 0; i < totalLoadBuffers; i++)
		{
			int static num = 1;

			if (this->loadBuffers[i].isBusy == false)
				continue;

			if (this->loadBuffers[i].instruction->instructionStatus.executionCyclesRemaining != 0)
				continue; //the instruction is still executing

			if (this->loadBuffers[i].instruction->instructionStatus.executionComplete == currentCycleNumber)
				continue; //the instruction has completed execution in the current cycle, so it cannot be written in the current Cycle

			this->loadBuffers[i].instruction->instructionStatus.writeBack = currentCycleNumber;
		
			reason += "-> The instruction at Load Buffer= " + this->loadBuffers[i].name + " has written back.\n";
			int regNum=atoi(&this->loadBuffers[i].instruction->Rt.c_str()[1]);
			if (this->registerStatus[regNum].writingUnit == this->loadBuffers[i].name)
				this->registerStatus[regNum].writingUnit = "";
			
			this->loadBuffers[i].isBusy = false;
			this->loadBuffers[i].address = "";
			this->loadBuffers[i].instruction = nullptr;
			
			char numChar[10];
			_itoa_s(num, numChar, 10);
			string val = "M(A";
			val.append(numChar);
			val += ")";
			broadCastResult(val, this->loadBuffers[i].name);
			num++;
		}


		for (int i = 0; i < totalStoreBuffers; i++)
		{
			if (this->storeBuffers[i].isBusy == false)
				continue;

			if (this->storeBuffers[i].instruction->instructionStatus.executionCyclesRemaining != 0)
				continue; //the instruction is still executing

			if (this->storeBuffers[i].instruction->instructionStatus.executionComplete == currentCycleNumber)
				continue; //the instruction has completed execution in the current cycle, so it cannot be written in the current Cycle

			reason += "-> The instruction at Store Buffer= " + this->storeBuffers[i].name + " has written back.\n";
			this->storeBuffers[i].instruction->instructionStatus.writeBack = currentCycleNumber;
			this->storeBuffers[i].isBusy = false;
			this->storeBuffers[i].address = "";
			this->storeBuffers[i].instruction = nullptr;
			//broadCastResult();
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////


		for (int i = 0; i < totalAddSubReservationStations; i++)
		{
			int static num = 1;

			if (this->addSubReservationStations[i].isBusy == false)
				continue;

			if (this->addSubReservationStations[i].instruction->instructionStatus.executionCyclesRemaining != 0)
				continue; //the instruction is still executing

			if (this->addSubReservationStations[i].instruction->instructionStatus.executionComplete == currentCycleNumber)
				continue; //the instruction has completed execution in the current cycle, so it cannot be written in the current Cycle

			this->addSubReservationStations[i].instruction->instructionStatus.writeBack = currentCycleNumber;
			reason += "-> The instruction at Reserrvation Station= " + this->addSubReservationStations[i].name + " has written back.\n";

			int regNum = atoi(&this->addSubReservationStations[i].instruction->Rd.c_str()[1]);
			if (this->registerStatus[regNum].writingUnit == this->addSubReservationStations[i].name)
				this->registerStatus[regNum].writingUnit = "";

			this->addSubReservationStations[i].isBusy = false;
			this->addSubReservationStations[i].instructionType = "";
			this->addSubReservationStations[i].Qj = "";
			this->addSubReservationStations[i].Qk = "";
			this->addSubReservationStations[i].Vj = "";
			this->addSubReservationStations[i].Vk = "";
			this->addSubReservationStations[i].instruction = nullptr;

			char numChar[10];
			_itoa_s(num, numChar, 10);
			string val = "V";
			val.append(numChar);
		
			broadCastResult(val, this->addSubReservationStations[i].name);
			num++;
		}

		////////////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < totalMultDivReservationStations; i++)
		{
			int static num = 1;

			if (this->multDivReservationStations[i].isBusy == false)
				continue;

			if (this->multDivReservationStations[i].instruction->instructionStatus.executionCyclesRemaining != 0)
				continue; //the instruction is still executing

			if (this->multDivReservationStations[i].instruction->instructionStatus.executionComplete == currentCycleNumber)
				continue; //the instruction has completed execution in the current cycle, so it cannot be written in the current Cycle

			this->multDivReservationStations[i].instruction->instructionStatus.writeBack = currentCycleNumber;
			reason += "-> The instruction at Reserrvation Station= " + this->multDivReservationStations[i].name + " has written back.\n";

			int regNum = atoi(&this->multDivReservationStations[i].instruction->Rd.c_str()[1]);
			if (this->registerStatus[regNum].writingUnit == this->multDivReservationStations[i].name)
				this->registerStatus[regNum].writingUnit = "";

			this->multDivReservationStations[i].isBusy = false;
			this->multDivReservationStations[i].instructionType = "";
			this->multDivReservationStations[i].Qj = "";
			this->multDivReservationStations[i].Qk = "";
			this->multDivReservationStations[i].Vj = "";
			this->multDivReservationStations[i].Vk = "";
			this->multDivReservationStations[i].instruction = nullptr;

			char numChar[10];
			_itoa_s(num, numChar, 10);
			string val = "V";
			val.append(numChar);

			broadCastResult(val, this->multDivReservationStations[i].name);
			num++;
		}


	}


	void execute()
	{
		for (int i = 0; i < totalLoadBuffers; i++)
		{
			if (this->loadBuffers[i].isBusy == false) 
				continue;

			if (this->loadBuffers[i].fu != "") 
				continue;//exeuction not started due to RAW hazard

			if (this->loadBuffers[i].instruction->instructionStatus.executionStart == -1)
			{   
				if (this->loadBuffers[i].instruction->instructionStatus.issue == currentCycleNumber)
					continue; //the instruction has been issued in the current cycle, so it cannot being instruction in the current cycle.

				//execution started
				this->loadBuffers[i].instruction->instructionStatus.executionStart = currentCycleNumber;
				this->loadBuffers[i].instruction->instructionStatus.executionCyclesRemaining--;
			

				if (this->loadBuffers[i].instruction->instructionStatus.executionCyclesRemaining == 0)
				{   //execution completed
					this->loadBuffers[i].instruction->instructionStatus.executionComplete = currentCycleNumber;
					reason += "-> The instruction at Load Buffer= " + this->loadBuffers[i].name + " has completed execution.\n";
					continue;
				}
				else
				{
					reason += "-> The instruction at Load Buffer= " + this->loadBuffers[i].name + " has started execution.\n";
					continue;
				}
			}
			
			if (this->loadBuffers[i].instruction->instructionStatus.executionCyclesRemaining!=0)
			this->loadBuffers[i].instruction->instructionStatus.executionCyclesRemaining--;
			else continue;

			if (this->loadBuffers[i].instruction->instructionStatus.executionCyclesRemaining == 0)
			{   //execution completed
				this->loadBuffers[i].instruction->instructionStatus.executionComplete = currentCycleNumber;
				reason += "-> The instruction at Load Buffer= " + this->loadBuffers[i].name + " has completed execution.\n";
				continue;
			}
			else
			{
				reason += "-> the instruction at Load Buffer= " + this->loadBuffers[i].name + " has completed one more execution cycle.\n";
				continue;
			}
			
			
		}//end of load buffers loop

		
		for (int i = 0; i < totalStoreBuffers; i++)
		{
			if (this->storeBuffers[i].isBusy == false)
				continue;

			if (this->storeBuffers[i].fu != "")
				continue;//exeuction not started due to RAW hazard

			if (this->storeBuffers[i].instruction->instructionStatus.executionStart == -1)
			{   
				if (this->storeBuffers[i].instruction->instructionStatus.issue == currentCycleNumber)
					continue; //the instruction has been issued in the current cycle, so it cannot start execution in the current cycle

				//execution started
				this->storeBuffers[i].instruction->instructionStatus.executionStart = currentCycleNumber;
				this->storeBuffers[i].instruction->instructionStatus.executionCyclesRemaining--;
				
				if (this->storeBuffers[i].instruction->instructionStatus.executionCyclesRemaining == 0)
				{   //execution completed
					this->storeBuffers[i].instruction->instructionStatus.executionComplete = currentCycleNumber;
					reason += "-> The instruction at Store Buffer= " + this->storeBuffers[i].name + " has completed execution.\n";
					continue;
				}
				else
				{
					reason += "-> The instruction at Store Buffer= " + this->storeBuffers[i].name + " has started execution.\n";
					continue;
				}
			}

			if (this->storeBuffers[i].instruction->instructionStatus.executionCyclesRemaining!=0)
			this->storeBuffers[i].instruction->instructionStatus.executionCyclesRemaining--;
			else continue;
			
			if (this->storeBuffers[i].instruction->instructionStatus.executionCyclesRemaining == 0)
			{   //execution completed
				this->storeBuffers[i].instruction->instructionStatus.executionComplete = currentCycleNumber;
				reason += "-> The instruction at Store Buffer= " + this->storeBuffers[i].name + " has completed execution.\n";
				continue;
			}
			else
			{
				reason += "-> the instruction at Store Buffer= " + this->storeBuffers[i].name + " has completed one more execution cycle.\n";
				continue;
			}

		}//end of store buffers loop

	
		for (int i = 0; i < totalAddSubReservationStations; i++)
		{
			if (this->addSubReservationStations[i].isBusy == false)
				continue;

			if (this->addSubReservationStations[i].Qj!="" || this->addSubReservationStations[i].Qk!="")
				continue;//exeuction not started due to RAW hazard

			if (this->addSubReservationStations[i].instruction->instructionStatus.executionStart == -1)
			{
				if (this->addSubReservationStations[i].instruction->instructionStatus.issue == currentCycleNumber)
					continue; //the instruction has been issued in the current cycle, so it cannot start execution in the current cycle

							  //execution started
				this->addSubReservationStations[i].instruction->instructionStatus.executionStart = currentCycleNumber;
				this->addSubReservationStations[i].instruction->instructionStatus.executionCyclesRemaining--;

				if (this->addSubReservationStations[i].instruction->instructionStatus.executionCyclesRemaining == 0)
				{   //execution completed
					this->addSubReservationStations[i].instruction->instructionStatus.executionComplete = currentCycleNumber;
					reason += "-> The instruction at Reservation Station= " + this->addSubReservationStations[i].name + " has completed execution.\n";
					continue;
				}
				else
				{
					reason += "-> The instruction at Reservation Station= " + this->addSubReservationStations[i].name + " has started execution.\n";
					continue;
				}
			}

			if (this->addSubReservationStations[i].instruction->instructionStatus.executionCyclesRemaining!=0)
			this->addSubReservationStations[i].instruction->instructionStatus.executionCyclesRemaining--;
			else continue;

			if (this->addSubReservationStations[i].instruction->instructionStatus.executionCyclesRemaining == 0)
			{   //execution completed
				this->addSubReservationStations[i].instruction->instructionStatus.executionComplete = currentCycleNumber;
				reason += "-> The instruction at Reservation Station= " + this->addSubReservationStations[i].name + " has completed execution.\n";
				continue;
			}
			else
			{
				reason += "-> the instruction at Reservation Station= " + this->addSubReservationStations[i].name + " has completed one more execution cycle.\n";
				continue;
			}

		}//end of addSub Reservation Stations loop



		////////////////////////////////////////////////////////////////////////
		for (int i = 0; i < totalMultDivReservationStations; i++)
		{
			if (this->multDivReservationStations[i].isBusy == false)
				continue;

			if (this->multDivReservationStations[i].Qj != "" || this->multDivReservationStations[i].Qk != "")
				continue;//exeuction not started due to RAW hazard

			if (this->multDivReservationStations[i].instruction->instructionStatus.executionStart == -1)
			{
				if (this->multDivReservationStations[i].instruction->instructionStatus.issue == currentCycleNumber)
					continue; //the instruction has been issued in the current cycle, so it cannot start execution in the current cycle

							  //execution started
				this->multDivReservationStations[i].instruction->instructionStatus.executionStart = currentCycleNumber;
				this->multDivReservationStations[i].instruction->instructionStatus.executionCyclesRemaining--;

				if (this->multDivReservationStations[i].instruction->instructionStatus.executionCyclesRemaining == 0)
				{   //execution completed
					this->multDivReservationStations[i].instruction->instructionStatus.executionComplete = currentCycleNumber;
					reason += "-> The instruction at Reservation Station= " + this->multDivReservationStations[i].name + " has completed execution.\n";
					continue;
				}
				else
				{
					reason += "-> The instruction at Reservation Station= " + this->multDivReservationStations[i].name + " has started execution.\n";
					continue;
				}
			}

			if (this->multDivReservationStations[i].instruction->instructionStatus.executionCyclesRemaining!=0)
			this->multDivReservationStations[i].instruction->instructionStatus.executionCyclesRemaining--;
			else continue;

			if (this->multDivReservationStations[i].instruction->instructionStatus.executionCyclesRemaining == 0)
			{   //execution completed
				this->multDivReservationStations[i].instruction->instructionStatus.executionComplete = currentCycleNumber;
				reason += "-> The instruction at Reservation Station= " + this->multDivReservationStations[i].name + " has completed execution.\n";
				continue;
			}
			else
			{
				reason += "-> the instruction at Reservation Station= " + this->multDivReservationStations[i].name + " has completed one more execution cycle.\n";
				continue;
			}

		}//end of multDiv Reservation Stations loop


	}

	void Simulate()
	{
		
		int currentInstructionNumberToBeIssued = 0;
		currentCycleNumber = 0;
	
	
	
		while (true)
		{
			gotoxy(0, 0);
			cout << "Current Cycle Number= " << currentCycleNumber;
			print();
			char a;
			cin >> a;
			currentCycleNumber++;
			system("cls");
			reason = "";

			int flag=issueInstruction(currentInstructionNumberToBeIssued);

			 if (flag != -1)
				 currentInstructionNumberToBeIssued++;

			execute();
			writeBack();
	
			
	
	    }

	}

	void print()
	{
		int y = 2;
		gotoxy(2, y);
		cout << "Instructions:";
		gotoxy(27, y);
		cout << "  Issue" << "   Start" << "  Finish     Write";
		gotoxy(27, y + 1);

		cout << "__________________________________";
		
		int j = 0;
		for (int i = 0; i < totalInstructions; i++)
		{
			char arr[10];
			_itoa_s(i, arr, 10);
			strcat_s(arr, ".");

			gotoxy(2, j + y + 2);
			if (instructions[i].instructionType == InstructionType::LOAD || instructions[i].instructionType == InstructionType::STORE)
				cout <<std::left<<setw(3)<<arr<< std::left << setw(5) << instructions[i].instructionType << setfill(' ') << std::right << setw(4) << instructions[i].Rt << setw(5) << instructions[i].immediateOffset << "+" << setw(5) << instructions[i].Rs;
			else
			{

				
				cout <<std::left << setw(3) <<arr<< std::left << setw(5) << instructions[i].instructionType << std::right << setw(4) << instructions[i].Rd << setw(5) << instructions[i].Rs << setw(6) << instructions[i].Rt;
			}
			


			gotoxy(2 + 24, j + y + 2);
			j++;
		    
			
		
			char instructionIssue[10];
			char executionStart[10];
			char executionComplete[10];
			char instructionWrite[10];

			_itoa_s(this->instructions[i].instructionStatus.issue, instructionIssue, 10);
			_itoa_s(this->instructions[i].instructionStatus.executionStart, executionStart, 10);
			_itoa_s(this->instructions[i].instructionStatus.executionComplete, executionComplete, 10);
			_itoa_s(this->instructions[i].instructionStatus.writeBack, instructionWrite, 10);

			cout << "|" << std::right << setw(7) << (instructions[i].instructionStatus.issue==-1 ? "" : instructionIssue) << "|" << setw(7) << (instructions[i].instructionStatus.executionStart==-1 ? "" : executionStart) << "|" << setw(7) << (instructions[i].instructionStatus.executionComplete==-1 ? "" :executionComplete) << "|" << setw(10) << (instructions[i].instructionStatus.writeBack==-1 ? "" : instructionWrite) << "|";
		
			gotoxy(2 + 24, j + y + 2);

			cout << "|_______|_______|_______|__________|";
			j++;
		}

		//printing Load Store Buffer
		gotoxy(70, y);
		cout << "Load/Store Buffer:   Busy      Addr       FU";
		y++;
		gotoxy(66 + 22 + 2, y);
		cout << "_____________________________";
		for (int i = 0; i < totalLoadBuffers; i++)
		{
			y++;;
			gotoxy(76 + 2, y);
			cout << setw(10) << std::right << loadBuffers[i].name;
			cout << " ";
			cout << "|" << std::right << setw(7) << (loadBuffers[i].isBusy == true ? "yes" : "no") << "|" << setw(10) << loadBuffers[i].address << "|" << setw(10) << loadBuffers[i].fu << "|";
			
			if (loadBuffers[i].instruction != nullptr)
				cout <<setw(3)<< loadBuffers[i].instruction->instructionStatus.executionCyclesRemaining;
			y++;
			gotoxy(76 + 2 + 11, y);
			cout << "|_______|__________|__________|" << endl;
		}

		for (int i = 0; i < totalStoreBuffers; i++)
		{
			y++;;
			gotoxy(76 + 2, y);
			cout << setw(10)<<setfill(' ') << std::right <<storeBuffers[i].name;
			cout << " ";
			cout << "|" << std::right << setw(7) << (storeBuffers[i].isBusy == true ? "yes" : "no") << "|" << setw(10) << storeBuffers[i].address << "|" << setw(10) << storeBuffers[i].fu << "|";
			if (storeBuffers[i].instruction != nullptr)
				cout << setw(3) << storeBuffers[i].instruction->instructionStatus.executionCyclesRemaining;

			y++;
			gotoxy(76 + 2 + 11, y);
			cout << "|_______|__________|__________|" << endl;
		}


		//reservation stations
		y = (y > j ? y : j);
		y += 5;
		gotoxy(4, y);
		cout << "Reservation Stations:";
		y++;
		gotoxy(21, y);
		cout << " Name   Busy    Op      Vj       Vk       Qj       Qk ";

		y++;
		gotoxy(21 + 7, y);
		cout << "_________________________________________________";
		for (int i = 0; i < totalAddSubReservationStations; i++)
		{
			y++;
			gotoxy(19, y);
			cout << std::right << setw(7) << addSubReservationStations[i].name << " |" << setw(5) << (addSubReservationStations[i].isBusy == true ? "yes" : "no") << "|" << setw(7) << addSubReservationStations[i].instructionType << "|" << setw(8) << addSubReservationStations[i].Vj << "|" << setw(8) << addSubReservationStations[i].Vk << "|" << setw(8) << addSubReservationStations[i].Qj << "|" << setw(8) << addSubReservationStations[i].Qk << "|";
			if (addSubReservationStations[i].instruction != nullptr)
			{
				cout << setw(3) << addSubReservationStations[i].instruction->instructionStatus.executionCyclesRemaining;
			}
			y++;
			gotoxy(19 + 7 + 1, y);
			cout << "|_____|_______|________|________|________|________|";
		}

		for (int i = 0; i < totalMultDivReservationStations; i++)
		{
			y++;
			gotoxy(19, y);
			cout << std::right << setw(7) << multDivReservationStations[i].name << " |" << setw(5) << (multDivReservationStations[i].isBusy == true ? "yes" : "no") << "|" << setw(7) << multDivReservationStations[i].instructionType << "|" << setw(8) << multDivReservationStations[i].Vj << "|" << setw(8) << multDivReservationStations[i].Vk << "|" << setw(8) << multDivReservationStations[i].Qj << "|" << setw(8) << multDivReservationStations[i].Qk << "|";
		
			if (multDivReservationStations[i].instruction != nullptr)
			{
				cout << setw(3) << multDivReservationStations[i].instruction->instructionStatus.executionCyclesRemaining;
			}
		
			y++;
			gotoxy(19 + 7 + 1, y);
			cout << "|_____|_______|________|________|________|________|";
		}

		y+=3;
		int x = 19;
	
		
		for (int i = 0; i < totalRegisters; i++)
		{
			gotoxy(x, y);
		   cout <<std::right<<setw(5)<< registerStatus[i].registerName ;
			
		   gotoxy(x+1, y + 1);
		
		   cout << "________";
		    gotoxy(x, y + 2);
			cout << "|" << setw(5) << registerStatus[i].writingUnit ;
	
			gotoxy(x, y + 3);
			cout << "|________";
			x += 5 + registerStatus[i].registerName.length() + 1;

	
		}
		gotoxy(x, y+2);
		cout << "|";
		gotoxy(x , y + 3);
		cout << "|";
		
		cout << endl << endl;
		cout << "What happend: " << endl << reason;

	}
	Tomasulo()
	{

	}
};
int main(int argc, char* argv[])
{
	
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;                   // Width of each character in the font
	cfi.dwFontSize.Y = 18;                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	wcscpy_s(cfi.FaceName, L"Consolas"); // Choose your font
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

 


	SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_FULLSCREEN_MODE, 0);
	COORD newSize = { 1000, 1000 };
	//SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), newSize);

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);

	system("cls");

	Tomasulo algorithm;
	//algorithm.instructions = new Instruction[5];
	//algorithm.totalInstructions = 5;

	////total execution cycles required
	//algorithm.totalLoadStoreCycles = 2;
	//algorithm.totalAddSubCycles = 2;
	//algorithm.totalMultCylcles = 5;
	//algorithm.totalDivCycles = 10;

	////DiVD instruction
	//algorithm.instructions[0].instructionType = InstructionType::DIVD;
	//algorithm.instructions[0].Rd = "R6";
	//algorithm.instructions[0].Rt = "R10";
	//algorithm.instructions[0].Rs = "R10";

	////ADDD Instruction
	//algorithm.instructions[1].instructionType = InstructionType::ADDD;
	//algorithm.instructions[1].Rs = "R7";
	//algorithm.instructions[1].Rt = "R6";
	//algorithm.instructions[1].Rd = "R1";

	////Store Instruction
	//algorithm.instructions[2].instructionType = InstructionType::STORE;
	//algorithm.instructions[2].Rs = "R2";
	//algorithm.instructions[2].Rt = "R1";
	//algorithm.instructions[2].immediateOffset = 400;

	////div instruction
	//algorithm.instructions[3].instructionType = InstructionType::DIVD;
	//algorithm.instructions[3].Rd = "R10";
	//algorithm.instructions[3].Rt = "R9";
	//algorithm.instructions[3].Rs = "R9";
	//
	////div instruction
	//algorithm.instructions[4].instructionType = InstructionType::DIVD;
	//algorithm.instructions[4].Rd = "R1";
	//algorithm.instructions[4].Rt = "R10";
	//algorithm.instructions[4].Rs = "R9";
	//


	////Load Store Buffers
	//algorithm.loadBuffers = new LoadStoreBuffer[2];
	//algorithm.loadBuffers[0].name = "LOAD1";
	//algorithm.loadBuffers[1].name = "LOAD2";
	//algorithm.totalLoadBuffers = 2;

	//algorithm.storeBuffers = new LoadStoreBuffer[2];
	//algorithm.storeBuffers[0].name = "STORE1";
	//algorithm.storeBuffers[1].name = "STORE2";
	//algorithm.totalStoreBuffers = 2;

	//
	////addSub reservation stations;
	//algorithm.addSubReservationStations = new ReservationStation[2];
	//algorithm.addSubReservationStations[0].name = "ADD1";
	//algorithm.addSubReservationStations[1].name = "ADD2";

	//algorithm.totalAddSubReservationStations = 1;
	//
	////multDiv ReservationStations
	//algorithm.multDivReservationStations = new ReservationStation[2];
	//algorithm.multDivReservationStations[0].name = "MULT1";
	//algorithm.multDivReservationStations[1].name = "MULT2";
	//algorithm.totalMultDivReservationStations = 1;

	////register status
	//algorithm.registerStatus = new RegisterStatus[11];
	//algorithm.totalRegisters = 11;
	//for (int i = 0; i < 11; i++)
	//{
	//	char buffer[10];
	//	_itoa_s(i, buffer, 10);
	//	string s="R";
	//	s.append(buffer);
	//	algorithm.registerStatus[i].registerName = s;
	//
	//}

	algorithm.loadDataFromFile("source.txt");
	
	algorithm.Simulate();

	system("pause");
}



void gotoxy(short x, short y)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
