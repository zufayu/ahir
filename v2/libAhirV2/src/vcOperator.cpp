#include <vcIncludes.hpp>
#include <vcRoot.hpp>
#include <vcType.hpp>
#include <vcValue.hpp>
#include <vcMemorySpace.hpp>
#include <vcControlPath.hpp>
#include <vcDataPath.hpp>
#include <vcOperator.hpp>
#include <vcModule.hpp>
#include <vcSystem.hpp>

vcEquivalence::vcEquivalence(string id, 
			     vector<vcWire*>& inwires, 
			     vector<vcWire*>& outwires):
  vcOperator(id)
{
  _flow_through = true;

  int in_width = 0;
  for(int idx = 0; idx < inwires.size(); idx++)
    {
      in_width += inwires[idx]->Get_Size();
      _inwires.push_back(inwires[idx]);
      inwires[idx]->Connect_Receiver(this);
    }

  int out_width = 0;
  for(int idx = 0; idx < outwires.size(); idx++)
    {
      out_width += outwires[idx]->Get_Size();
      _outwires.push_back(outwires[idx]);
      outwires[idx]->Connect_Driver(this);
    }

  if(in_width != out_width)
    {
      vcSystem::Warning("in equivalence operator " + id + ", total input width is not equal to total output width.. output will truncate the input..");
    }

  _in_width = in_width;
  _out_width = out_width;
}

void vcEquivalence::Print(ostream& ofile)
{
  ofile << vcLexerKeywords[__EQUIVALENCE_OP] << " " << this->Get_Label() << " ";
  ofile << vcLexerKeywords[__LPAREN];
  for(int idx = 0; idx < _inwires.size(); idx++)
    {
      if(idx > 0)
	ofile << " ";
      ofile << _inwires[idx]->Get_Id();
    }
  ofile << vcLexerKeywords[__RPAREN];
  ofile << " " 	<< vcLexerKeywords[__LPAREN];
  for(int idx = 0; idx < _outwires.size(); idx++)
    {
      if(idx > 0)
	ofile << " ";
      ofile << _outwires[idx]->Get_Id();
    }
  ofile << vcLexerKeywords[__RPAREN] << " ";

  this->Print_Guard(ofile);
  this->Print_Flow_Through(ofile);
  ofile << endl;

  this->Print_Delay(ofile);
}


void   vcOperator::Add_Reqs(vector<vcTransition*>& reqs)
{
  assert(reqs.size() == 1);
  this->_reqs = reqs;
}

void vcOperator::Add_Acks(vector<vcTransition*>& acks)
{
  assert(acks.size() == 1);
  this->_acks = acks;
}


void vcOperator::Print_VHDL_Logger(ostream& ofile)
{
	string req_id = this->_reqs[0]->Get_CP_To_DP_Symbol();
	string ack_id = this->_acks[0]->Get_DP_To_CP_Symbol();
	bool guard_present = (this->Get_Guard_Wire() != NULL);

	string guard_id;

	if(this->Get_Guard_Wire() != NULL)
		guard_id = this->Get_Guard_Wire()->Get_VHDL_Id() + "(0)";
	else
		guard_id = "sl_one";
		
	ofile  << "LogOperator(clk,reset,global_clock_cycle_count,";
	ofile  << req_id << "," << ack_id << "," << guard_id << "," 
		<< '"' << this->Get_Id() << " " << this->Get_Logger_Description() <<  '"' 
		<< "," ;
	if(this->Get_Number_Of_Input_Wires() > 0)
	{
		ofile << "false,"; // ignore input flag is set false.
		for(int idx = 0, fidx = this->Get_Number_Of_Input_Wires(); idx < fidx; idx++)
		{
			if(idx != 0)
				ofile << " & ";
			ofile  <<  this->Get_Input_Wire(idx)->Get_VHDL_Signal_Id();
		}
	}
	else
	{
		ofile << "true, slv_zero"; // ignore inputs in logger.
	}

	ofile << "," << endl;

	if(this->Get_Number_Of_Output_Wires() > 0)
	{
		ofile << "false,"; // ignore output flag is set false.
		for(int idx = 0, fidx = this->Get_Number_Of_Output_Wires(); idx < fidx; idx++)
		{
			if(idx != 0)
				ofile << " & ";
			ofile  <<  this->Get_Output_Wire(idx)->Get_VHDL_Signal_Id();
		}
	}
	else
	{
		ofile << "true, slv_zero"; // ignore outputs in logger.
	}
	ofile << ");" << endl;
}

void   vcSplitOperator::Add_Reqs(vector<vcTransition*>& reqs)
{
  assert(reqs.size() == 2);
  this->_reqs = reqs;
}

void vcSplitOperator::Add_Acks(vector<vcTransition*>& acks)
{
  assert(acks.size() == 2);
  this->_acks = acks;
}

void vcSplitOperator::Print_VHDL_Logger(ostream& ofile)
{
	string req0_id = this->_reqs[0]->Get_CP_To_DP_Symbol();
	string req1_id = this->_reqs[1]->Get_CP_To_DP_Symbol();
	string ack0_id = this->_acks[0]->Get_DP_To_CP_Symbol();
	string ack1_id = this->_acks[1]->Get_DP_To_CP_Symbol();

	bool guard_present = (this->Get_Guard_Wire() != NULL);

	string guard_id;

	if(this->Get_Guard_Wire() != NULL)
		guard_id = this->Get_Guard_Wire()->Get_VHDL_Id() + "(0)";
	else
		guard_id = "sl_one";

		
	ofile  << "LogSplitOperator(clk,reset,global_clock_cycle_count," ;
	ofile  << req0_id << "," << ack0_id << ",";
	ofile  << req1_id << "," << ack1_id << "," << guard_id << "," 
		<< '"' << this->Get_Id() << '"' 
		<< "," ;

	if(this->Get_Number_Of_Input_Wires() > 0)
	{
		ofile << "false,"; // ignore input flag is set false.
		for(int idx = 0, fidx = this->Get_Number_Of_Input_Wires(); idx < fidx; idx++)
		{
			if(idx != 0)
				ofile << " & ";
			ofile  <<  this->Get_Input_Wire(idx)->Get_VHDL_Signal_Id();
		}
	}
	else
	{
		ofile << "true, slv_zero"; // ignore inputs in logger.
	}

	ofile << "," << endl;

	if(this->Get_Number_Of_Output_Wires() > 0)
	{
		ofile << "false,"; // ignore output flag is set false.
		for(int idx = 0, fidx = this->Get_Number_Of_Output_Wires(); idx < fidx; idx++)
		{
			if(idx != 0)
				ofile << " & ";
			ofile  <<  this->Get_Output_Wire(idx)->Get_VHDL_Signal_Id();
		}
	}
	else
	{
		ofile << "true, slv_zero"; // ignore outputs in logger.
	}
	ofile << ");" << endl;
}

vcLoadStore::vcLoadStore(string id, vcMemorySpace* ms, vcWire* addr, vcWire* data):vcSplitOperator(id)
{
  _memory_space = ms;
  _address = addr;
  _data = data;

  
  assert(ms != NULL);

  if(ms->Get_Address_Width() != addr->Get_Type()->Size())
    {
      string err_msg = string("load/store operator address-wire width must\n") +
	"be the same as the memory space address width\n" +
	"for load/store with id " + id + 
	" (memory space " + ms->Get_Id() + ")"; 

      vcSystem::Error(err_msg);

    }
  if(ms->Get_Word_Size() != data->Get_Type()->Size())
    {
      string err_msg = string("load/store operator data-wire width must\n") +
	"be the same as the memory space data width\n" +
	"for load/store with id " + id + 
	" (memory space " + ms->Get_Id() + ")";
      vcSystem::Error(err_msg);
    }
  
}


vcLoad::vcLoad(string id, vcMemorySpace* ms, vcWire* addr, vcWire* data):vcLoadStore(id,ms,addr,data) 
{
  addr->Connect_Receiver(this);
  data->Connect_Driver(this);
}
void vcLoad::Print(ostream& ofile)
{
  ofile << vcLexerKeywords[__LOAD] 	<< " "
	<< this->Get_Label() << " " << vcLexerKeywords[__FROM] << " "
	<< this->_memory_space->Get_Hierarchical_Id() << " "
	<< vcLexerKeywords[__LPAREN] << " "
	<< this->_address->Get_Id() << " "
	<< vcLexerKeywords[__RPAREN] << " "
	<< vcLexerKeywords[__LPAREN] << " "
	<< this->_data->Get_Id() <<  " "
	<< vcLexerKeywords[__RPAREN] << " ";
  this->Print_Guard(ofile);
  ofile << endl;
  this->Print_Delay(ofile);
}
  
void vcLoad::Append_Inwire_Buffering(vector<int>& inwire_buffering)
{
	inwire_buffering.push_back(this->Get_Input_Buffering(_address));
}

void vcLoad::Append_Outwire_Buffering(vector<int>& outwire_buffering)
{
	outwire_buffering.push_back(this->Get_Output_Buffering(_data));
}

vcStore::vcStore(string id, vcMemorySpace* ms, vcWire* addr, vcWire* data):vcLoadStore(id,ms,addr,data) 
{
  addr->Connect_Receiver(this);
  data->Connect_Receiver(this);
}

// max of both inputs.
void vcStore::Append_Inwire_Buffering(vector<int>& inwire_buffering)
{
	int ba = this->Get_Input_Buffering(_address);
	int bd = this->Get_Input_Buffering(_data);
	inwire_buffering.push_back((ba > bd) ? ba : bd);
}

void vcStore::Print(ostream& ofile)
{
  ofile << vcLexerKeywords[__STORE] 	<< " "
	<< this->Get_Label() << " " << vcLexerKeywords[__TO] << " "
	<< this->_memory_space->Get_Hierarchical_Id() << " "
	<< vcLexerKeywords[__LPAREN] << " "
	<< this->_address->Get_Id() << " "
	<< this->_data->Get_Id() << " "
	<< vcLexerKeywords[__RPAREN] << " ";
  this->Print_Guard(ofile);
  ofile << endl;
  this->Print_Delay(ofile);
}

vcPhi::vcPhi(string id, vector<vcWire*>& inwires, vcWire* outwire):vcDatapathElement(id)
{
  assert(inwires.size() > 0 && outwire != NULL);

  // all wires must have the same type
  vcType* t = outwire->Get_Type();
  for(int idx = 0; idx < inwires.size(); idx++)
    assert(t == inwires[idx]->Get_Type());

  this->_inwires = inwires;
  for(int idx =0; idx < _inwires.size(); idx++)
    _inwires[idx]->Connect_Receiver(this);

  this->_outwire = outwire;
  _outwire->Connect_Driver(this);

}

void vcPhi::Add_Reqs(vector<vcTransition*>& reqs)
{
  assert(reqs.size() > 0);
  this->_reqs = reqs;
}

void vcPhi::Add_Acks(vector<vcTransition*>& acks)
{
  assert(acks.size() == 1);
  this->_acks = acks;
}

void vcPhi::Print(ostream& ofile)
{
  ofile << vcLexerKeywords[__PHI] << " " << this->Get_Label() << " ";
  ofile << vcLexerKeywords[__LPAREN];
  for(int idx = 0; idx < this->_inwires.size(); idx++)
    {
      ofile << this->_inwires[idx]->Get_Id() << " ";
    }
  ofile << vcLexerKeywords[__RPAREN] 
	<< " ";
  ofile << vcLexerKeywords[__LPAREN];
  ofile << this->_outwire->Get_Id();
  ofile << vcLexerKeywords[__RPAREN] ;
  ofile << endl;
  this->Print_Delay(ofile);
}

void vcPhi::Print_VHDL(ostream& ofile)
{
      int odata_width = this->Get_Outwire()->Get_Type()->Size();
      int num_reqs = this->Get_Inwires().size();
      int idata_width = num_reqs*odata_width;

      if((this->Get_Number_Of_Reqs() < num_reqs) ||
	 (this->Get_Number_Of_Acks() != 1))
	{
	  vcSystem::Error("phi operator " + this->Get_Id() + " has inadequate/incorrect req-ack links");
	  return;
	}

      ofile << this->Get_VHDL_Id() << ": Block -- phi operator {" << endl;
      ofile << "signal idata: std_logic_vector(" << idata_width-1 << " downto 0);" << endl;
      ofile << "signal req: BooleanArray(" << num_reqs-1 << " downto 0);" << endl;
      ofile << "--}\n begin -- {" << endl;
      ofile << "idata <= ";
      for(int idx = 0; idx < this->Get_Inwires().size(); idx++)
	{
	  if(idx > 0)
	    ofile << " & ";
	  ofile << this->Get_Inwires()[idx]->Get_VHDL_Signal_Id();
	}
      ofile << ";" << endl;

      if(this->Get_Number_Of_Reqs() == 1)
      {
        ofile << "req(0) <= ";
	ofile << this->Get_Req(0)->Get_CP_To_DP_Symbol();
      }
      else
      {
        ofile << "req <= ";
      	for(int idx = 0; idx < this->Get_Number_Of_Reqs(); idx++)
	{
	  if(idx > 0)
	    ofile << " & ";
	  ofile << this->Get_Req(idx)->Get_CP_To_DP_Symbol();
	}
      }
      ofile << ";" << endl;
      
      ofile << "phi: PhiBase -- {" << endl
	    << "generic map( -- { " << endl 
	    << "num_reqs => " << this->Get_Number_Of_Reqs() << "," << endl
	    << "data_width => " << odata_width << ") -- }"  << endl
	    << "port map( -- { "<< endl 
	    << "req => req, " << endl
	    << "ack => " << this->Get_Ack(0)->Get_DP_To_CP_Symbol()  << "," << endl
	    << "idata => idata," << endl
	    << "odata => " << this->Get_Outwire()->Get_VHDL_Signal_Id() << "," << endl
	    << "clk => clk," << endl
	    << "reset => reset ); -- }}" << endl;
      ofile << "-- }\n end Block; -- phi operator " << this->Get_VHDL_Id() << endl;
}


vcPhiPipelined::vcPhiPipelined(string id, vector<vcWire*>& inwires, vcWire* outwire): vcPhi(id,inwires,outwire) 
{
}

void vcPhiPipelined::Print(ostream& ofile)
{
  ofile << vcLexerKeywords[__HASH] << " ";
  this->vcPhi::Print(ofile);
}

void vcPhiPipelined::Add_Acks(vector<vcTransition*>& acks)
{
  assert(acks.size() == 2);
  this->_acks = acks;
}
void vcPhiPipelined::Print_VHDL(ostream& ofile)
{
      int odata_width = this->Get_Outwire()->Get_Type()->Size();
      int num_reqs = this->Get_Inwires().size();
      int idata_width = num_reqs*odata_width;
      int buffering = 0;

      if((this->Get_Number_Of_Reqs() != (num_reqs+1)) ||
	 (this->Get_Number_Of_Acks() != 2))
	{
	  vcSystem::Error("pipelined-phi operator " + this->Get_Id() + " has inadequate/incorrect req-ack links");
	  return;
	}

      assert(this->Get_Number_Of_Output_Wires() ==  1);
      buffering = this->Get_Output_Buffering(this->Get_Output_Wire(0));

      ofile << this->Get_VHDL_Id() << ": Block -- pipelined-phi operator {" << endl;
      ofile << "signal idata: std_logic_vector(" << idata_width-1 << " downto 0);" << endl;
      ofile << "signal sample_req: BooleanArray(" << num_reqs-1 << " downto 0);" << endl;
      ofile << "signal sample_ack, update_req, update_ack: Boolean;" << endl;
      ofile << "--}\n begin -- {" << endl;
      ofile << "idata <= ";
      for(int idx = 0; idx < this->Get_Number_Of_Input_Wires(); idx++)
	{
	  vcWire* w = this->Get_Input_Wire(idx);
	  if(idx > 0)
	    ofile << " & ";
	  ofile << w->Get_VHDL_Signal_Id();
	  int U = this->Get_Input_Buffering(w);
	  buffering = ((buffering < U) ? U : buffering); 
	}
      ofile << ";" << endl;

      ofile << "sample_req <= ";
      for(int idx = 0; idx < this->Get_Number_Of_Reqs()-1; idx++)
      {
	      if(idx > 0)
		      ofile << " & ";
	      ofile << this->Get_Req(idx)->Get_CP_To_DP_Symbol();
      }
      ofile << ";" << endl;
      ofile << "update_req <= " <<  this->Get_Req(this->Get_Number_Of_Reqs()-1)->Get_CP_To_DP_Symbol() << ";" << endl;

      ofile << "phi: PhiPipelined -- {" << endl
	    << "generic map( -- { " << endl 
	    << "name => \"" << this->Get_VHDL_Id() << "\"," << endl
	    << "buffering => " << buffering << "," << endl
	    << "num_reqs => " << this->Get_Number_Of_Reqs()-1 << "," << endl
	    << "data_width => " << odata_width << ") -- }"  << endl
	    << "port map( -- { "<< endl 
	    << "sample_req => sample_req, " << endl
	    << "sample_ack => " << this->Get_Ack(0)->Get_DP_To_CP_Symbol()  << "," << endl
	    << "update_req => update_req," << endl
	    << "update_ack => " << this->Get_Ack(1)->Get_DP_To_CP_Symbol()  << "," << endl
	    << "idata => idata," << endl
	    << "odata => " << this->Get_Outwire()->Get_VHDL_Signal_Id() << "," << endl
	    << "clk => clk," << endl
	    << "reset => reset ); -- }}" << endl;
      ofile << "-- }\n end Block; -- phi operator " << this->Get_VHDL_Id() << endl;
}

vcCall::vcCall(string id, vcModule* m, vector<vcWire*>& in_wires, vector<vcWire*> out_wires, bool inline_flag):vcSplitOperator(id)
{
	_called_module = m;


	_in_wires = in_wires;
	for(int idx =0; idx < _in_wires.size(); idx++)
		_in_wires[idx]->Connect_Receiver(this);

	_out_wires = out_wires;
	for(int idx =0; idx < _out_wires.size(); idx++)
		_out_wires[idx]->Connect_Driver(this);

	_inline_flag = inline_flag;
}

int vcCall::Get_Delay()
{
	bool is_lib_mod = _called_module->Get_Is_Function_Library_Module();
	if(is_lib_mod)
		return(_called_module->Get_Delay());
	else
		return(_delay);
}

// Calls will use a value for the input wire buffering which
// is the maximum specified across all input wires.
void vcCall::Append_Inwire_Buffering(vector<int>& inwire_buffering)
{
	int max_buf = 0;
	for(int idx = 0; idx < _in_wires.size(); idx++)
	{
		int U = this->Get_Input_Buffering(_in_wires[idx]);
		max_buf = ((U > max_buf) ? U : max_buf);
	}
	inwire_buffering.push_back(max_buf);
}

// Calls will use a value for the output wire buffering which
// is the maximum specified across all output wires.
void vcCall::Append_Outwire_Buffering(vector<int>& outwire_buffering)
{
	int max_buf = 0;
	for(int idx = 0; idx < _out_wires.size(); idx++)
	{
		int U = this->Get_Output_Buffering(_out_wires[idx]);
		max_buf = ((U > max_buf) ? U : max_buf);
	}
	outwire_buffering.push_back(max_buf);
}

void vcCall::Print(ostream& ofile)
{
	ofile << vcLexerKeywords[__CALL] << " "
		<< (_inline_flag ? vcLexerKeywords[__INLINE] : "") <<  " " 
		<< this->Get_Label() << " "
		<< vcLexerKeywords[__MODULE] << " "
		<< this->_called_module->Get_Id() << " ";

	// input arguments.
	ofile << vcLexerKeywords[__LPAREN] << " ";
	for(int idx = 0; idx < _in_wires.size(); idx++)
		ofile << _in_wires[idx]->Get_Id() << " ";
	ofile << vcLexerKeywords[__RPAREN] << " ";


	// output arguments.
	ofile << vcLexerKeywords[__LPAREN] << " ";
	for(int idx = 0; idx < _out_wires.size(); idx++)
		ofile << _out_wires[idx]->Get_Id() << " ";
	ofile << vcLexerKeywords[__RPAREN] << " ";
	this->Print_Guard(ofile);
	this->Print_Flow_Through(ofile);
	ofile << endl;
	this->Print_Delay(ofile);
}

vcIOport::vcIOport(string id, vcPipe* pipe, vcWire* w): vcOperator(id)
{
	_pipe = pipe;
	_data = w;
}

string vcIOport::Get_Pipe_Id()
{
	return(this->_pipe->Get_Id());
}

vcInport::vcInport(string id, vcPipe* pipe, vcWire* w):vcIOport(id,pipe,w) 
{
	w->Connect_Driver(this);
}

void   vcInport::Add_Reqs(vector<vcTransition*>& reqs)
{
	assert(reqs.size() == 2);
	this->_reqs = reqs;
}

void vcInport::Add_Acks(vector<vcTransition*>& acks)
{
	assert(acks.size() == 2);
	this->_acks = acks;
}

void vcInport::Append_Outwire_Buffering(vector<int>& outwire_buffering)
{
	outwire_buffering.push_back(this->Get_Output_Buffering(_data));
}

void vcInport::Print(ostream& ofile)
{
  ofile << vcLexerKeywords[__IOPORT] << " " <<  vcLexerKeywords[__IN] << " " << this->Get_Label() << "  " 
	<< vcLexerKeywords[__LPAREN] << this->Get_Pipe_Id() << vcLexerKeywords[__RPAREN] << " "
	<< vcLexerKeywords[__LPAREN] << this->_data->Get_Id() << vcLexerKeywords[__RPAREN] << " ";
  this->Print_Guard(ofile);
  ofile << endl;
  this->Print_Delay(ofile);
}

vcOutport::vcOutport(string id, vcPipe* pipe, vcWire* w):vcIOport(id,pipe,w) 
{
  w->Connect_Receiver(this);
}
void vcOutport::Print(ostream& ofile)
{
  ofile << vcLexerKeywords[__IOPORT] << " " <<  vcLexerKeywords[__OUT] << " " << this->Get_Label() << "  " 
	<< vcLexerKeywords[__LPAREN] << this->_data->Get_Id() << vcLexerKeywords[__RPAREN] << " " 
	<< vcLexerKeywords[__LPAREN] << this->Get_Pipe_Id() << vcLexerKeywords[__RPAREN] << " ";
  this->Print_Guard(ofile);
  ofile << endl;
  this->Print_Delay(ofile);
}

void   vcOutport::Add_Reqs(vector<vcTransition*>& reqs)
{
  assert(reqs.size() == 2);
  this->_reqs = reqs;
}

void vcOutport::Add_Acks(vector<vcTransition*>& acks)
{
  assert(acks.size() == 2);
  this->_acks = acks;
}

vcUnarySplitOperator::vcUnarySplitOperator(string id, string op_id, vcWire* x, vcWire* z):vcSplitOperator(id)
{
  
  assert(x != NULL && z != NULL);
  
  this->_op_id = op_id;

  this->_x = x;
  x->Connect_Receiver(this);

  this->_z = z;
  z->Connect_Driver(this);
}

bool vcUnarySplitOperator::Is_Shareable_With(vcDatapathElement* other)
{
  // unary operations are too trivial to share..
  return(false);
}


void vcUnarySplitOperator::Append_Inwire_Buffering(vector<int>& inwire_buffering)
{
	inwire_buffering.push_back(this->Get_Input_Buffering(_x));
}

void vcUnarySplitOperator::Append_Outwire_Buffering(vector<int>& outwire_buffering)
{
	outwire_buffering.push_back(this->Get_Output_Buffering(_z));
}

void vcUnarySplitOperator::Print(ostream& ofile)
{
  ofile << this->_op_id << " " << this->Get_Label() << " "
	<< vcLexerKeywords[__LPAREN] 
	<< this->_x->Get_Id() << " "
	<< vcLexerKeywords[__RPAREN] 
	<< " "
	<< vcLexerKeywords[__LPAREN] 
	<< this->_z->Get_Id()
	<< vcLexerKeywords[__RPAREN] 
	<<  " ";
  this->Print_Guard(ofile);
	this->Print_Flow_Through(ofile);
  ofile << endl;
  this->Print_Delay(ofile);
}



vcBinarySplitOperator::vcBinarySplitOperator(string id, string op_id, vcWire* x, vcWire* y, vcWire* z):vcSplitOperator(id)
{
  assert(x != NULL && y != NULL && z != NULL);
  
  this->_op_id = op_id;


  if(x->Is("vcConstantWire"))
    {
      // both cannot be constants!
      assert(!y->Is("vcConstantWire"));
      if(Is_Symmetric_Op(op_id))
	{
	  vcWire* tmp = y;
	  y = x;
	  x = tmp;
	}
    }
  
  this->_x = x;
  x->Connect_Receiver(this);

  this->_y = y;
  y->Connect_Receiver(this);

  this->_z = z;
  z->Connect_Driver(this);
}

bool vcBinarySplitOperator::Is_Pipelined_Operator()
{
  vcType* input_type =   this->Get_Input_Type();
  vcType* output_type =  this->Get_Output_Type();
  string vc_op_id = this->Get_Op_Id();
  int exp_width, frac_width;  // wasted.
  bool is_pipelined_float_op = Is_Pipelined_Float_Op(vc_op_id, input_type, output_type, exp_width, frac_width);

  return(is_pipelined_float_op);
}

bool vcBinarySplitOperator::Is_Shareable_With(vcDatapathElement* other)
{

  // trivial operations are not worth sharing..
  if(Is_Trivial_Op(this->_op_id))
    return(false);

  // lets try not sharing integer adders at all..
  if(this->Is_Int_Add_Op())
    return(false);

  // Compare ops are also too simple to share..
  if(Is_Compare_Op(this->_op_id))
    return(false);

  bool ret_val = ((this->Kind() == other->Kind()) 
		  && 
		  (this->_op_id == ((vcBinarySplitOperator*)other)->Get_Op_Id()));
  if(!ret_val)
    return(ret_val);

  // a hack for FP pipelined operators, this is to get around 
  // the fact that constant operands are not treated separately.
  // for pipelined operators..
  bool is_pipelined_op = this->Is_Pipelined_Operator();
  if(is_pipelined_op)
    {
      vcType* input_type =   this->Get_Input_Type();
      vcType* output_type =  this->Get_Output_Type();

      // other and this are of the same kind..
      vcBinarySplitOperator* so = (vcBinarySplitOperator*) other;

      if((input_type == so->Get_Input_Type()) && (output_type == so->Get_Output_Type()))
	return(true);
      else
	return(false);
    }

  // check for compatibility of wires.. constants have to match by value.
  vector<vcWire*> iw1, iw2;
  this->Append_Inwires(iw1);
  other->Append_Inwires(iw2);

  if(!Check_If_Equivalent(iw1,iw2))
    return(false);

  vector<vcWire*> ow1, ow2;
  this->Append_Outwires(iw1);
  other->Append_Outwires(iw2);

  if(!Check_If_Equivalent(ow1,ow2))
    return(false);

  if(_y->Is("vcConstantWire") && Is_Shift_Op(this->_op_id))
    return(false);
  
  if((_y->Is("vcConstantWire") || _x->Is("vcConstantWire")) && this->Is_Int_Add_Op())
    return(false);

  if(_y->Is("vcConstantWire") && (((vcBinarySplitOperator*)other)->Get_Y()->Is("vcConstantWire")))
    {
      vcConstantWire* cy = (vcConstantWire*) _y;
      vcValue* cyv = cy->Get_Value();
      
      vcConstantWire* coy = (vcConstantWire*) ((vcBinarySplitOperator*) other)->Get_Y();
      vcValue* coyv = coy->Get_Value();

      if(cyv->Kind() == coyv->Kind())
	{
	  if(cyv->Get_Type()->Is_Int_Type())
	    {
	      if(!(*((vcIntValue*)cyv) == *((vcIntValue*) coyv)))
		return(false);
	    }
	  else
	    {
	      if(!(*((vcFloatValue*)cyv) == *((vcFloatValue*) coyv)))
		return(false);
	    }
	}
    }

  return(true);
}


// shared binary operators will use a value of input buffering
// which is the maximum specified across all input wires.
void vcBinarySplitOperator::Append_Inwire_Buffering(vector<int>& inwire_buffering)
{
	int bx = this->Get_Input_Buffering(_x);
	int by = this->Get_Input_Buffering(_y);
	inwire_buffering.push_back((bx > by) ? bx : by);
}

void vcBinarySplitOperator::Append_Outwire_Buffering(vector<int>& outwire_buffering)
{
	outwire_buffering.push_back(this->Get_Output_Buffering(_z));
}

void vcBinarySplitOperator::Print(ostream& ofile)
{
  ofile << this->_op_id << " " << this->Get_Label() << " "
	<< vcLexerKeywords[__LPAREN] 
	<< this->_x->Get_Id() << " "
	<< this->_y->Get_Id() << " "
	<< vcLexerKeywords[__RPAREN] 
	<< " "
	<< vcLexerKeywords[__LPAREN] 
	<< this->_z->Get_Id()
	<< vcLexerKeywords[__RPAREN] 
	<<  " ";
  this->Print_Guard(ofile);
	this->Print_Flow_Through(ofile);
  ofile << endl;
  this->Print_Delay(ofile);
}

vcSelect::vcSelect(string id, vcWire* sel, vcWire* x, vcWire* y, vcWire* z):vcSplitOperator(id)
{
  this->_x = x;
  _x->Connect_Receiver(this);

  this->_y = y;
  _y->Connect_Receiver(this);

  this->_sel = sel;
  _sel->Connect_Receiver(this);

  this->_z = z;
  _z->Connect_Driver(this);
}

void vcSelect::Print(ostream& ofile)
{
  ofile << vcLexerKeywords[__SELECT_OP] << " " << this->Get_Label() << " "
        << vcLexerKeywords[__LPAREN]
        << this->_sel->Get_Id() << " "
        << this->_x->Get_Id() << " "
        << this->_y->Get_Id() << " "
        << vcLexerKeywords[__RPAREN]
        << " "
        << vcLexerKeywords[__LPAREN]
        << this->_z->Get_Id()
        << vcLexerKeywords[__RPAREN]
        <<  " ";
  this->Print_Guard(ofile);
	this->Print_Flow_Through(ofile);
  ofile << endl;
  this->Print_Delay(ofile);
}


void vcSelect::Print_VHDL(ostream& ofile)
{
        string inst_name = this->Get_VHDL_Id();
	string block_name = inst_name + "_block"; 
        string name = '"' + inst_name + '"';
	bool flow_through = this->Get_Flow_Through();

	ofile << block_name << " : block -- { " << endl;
	ofile << "signal sample_req, sample_ack, update_req, update_ack: BooleanArray(0 downto 0); " << endl;
	if(!flow_through && (this->_guard_wire != NULL))
	{
		ofile << " signal sample_req_ug, sample_ack_ug, update_req_ug, update_ack_ug: BooleanArray(0 downto 0); " << endl;
		ofile << " signal guard_vector : std_logic_vector(0 downto 0); " << endl;

		string buf_const;
		string gflags;
		vector<vcDatapathElement*> ops;
		vector<vcWire*> gwires;	
		
		ops.push_back(this);
		gwires.push_back(this->_guard_wire);
		Generate_Guard_Constants(buf_const, gflags, ops, gwires);

		ofile << buf_const << endl;
		ofile << gflags << endl;
	}
	ofile << " -- } " << endl;
      ofile << "begin -- { " << endl;
 

      if(!flow_through and (this->Get_Guard_Wire() != NULL))
      {
        	ofile << " sample_req_ug(0) <= "   << this->Get_Req(0)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(0)->Get_DP_To_CP_Symbol() << "<= sample_ack_ug(0);" << endl;
        	ofile << " update_req_ug(0) <= "   << this->Get_Req(1)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(1)->Get_DP_To_CP_Symbol() << "<= update_ack_ug(0);" << endl;
		ofile << " guard_vector(0) <= " << this->_guard_wire->Get_VHDL_Signal_Id() << "(0);" << endl; 
      }
      else
      {
        	ofile << " sample_req(0) <= "   << this->Get_Req(0)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(0)->Get_DP_To_CP_Symbol() << "<= sample_ack(0);" << endl;
        	ofile << " update_req(0) <= "   << this->Get_Req(1)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(1)->Get_DP_To_CP_Symbol() << "<= update_ack(0);" << endl;
      }

      vector<int> buf_sizes;
      buf_sizes.push_back(this->Get_Input_Buffering(this->_x));
      buf_sizes.push_back(this->Get_Input_Buffering(this->_y));
      buf_sizes.push_back(this->Get_Input_Buffering(this->_sel));
      buf_sizes.push_back(this->Get_Output_Buffering(this->_z));
      int buf_size = max(buf_sizes);

      if(!flow_through && (this->_guard_wire != NULL))
      {
	      string guards = "guard_vector";
	      vector<vcWire*> gwires;
	      gwires.push_back(this->_guard_wire);
	      Print_VHDL_Guard_Instance("gI", 1, "guardBuffering", "guardFlags", "guard_vector",
			      "sample_req_ug", "sample_ack_ug", "sample_req", "sample_ack",
			      "update_req_ug", "update_ack_ug", "update_req", "update_ack", ofile);
      }

      
      ofile << this->Get_VHDL_Id() << ": SelectSplitProtocol generic map(name => " << name 
		<< ", data_width => " << this->_z->Get_Size()  << "," 
		<< " buffering => " << buf_size << ", "
		<< "flow_through => " << (flow_through ? "true" : "false") 
		<< ") -- {" << endl;
      ofile << " port map( x => " 
	    << this->_x->Get_VHDL_Signal_Id() 
	    << ", y => " 
	    << this->_y->Get_VHDL_Signal_Id() 
	    << ", sel => " 
	    << this->_sel->Get_VHDL_Signal_Id() 
	    << ", z => " << this->_z->Get_VHDL_Signal_Id() 
	    << ", sample_req => sample_req(0)"
	    << ", sample_ack => sample_ack(0)"
	    << ", update_req => update_req(0)"
	    << ", update_ack => update_ack(0)"
	    << ", clk => clk, reset => reset); -- }" << endl;

      ofile << "-- }" << endl << "end block;" << endl;
}

vcSlice::vcSlice(string id, vcWire* din, vcWire* dout, int high_index, int low_index):vcSplitOperator(id)
{
  this->_din = din;
  _din->Connect_Receiver(this);

  this->_dout = dout;
  _dout->Connect_Driver(this);

  assert(high_index < din->Get_Size() && low_index >= 0 && (high_index >= low_index));
  assert(dout->Get_Size() == ((high_index - low_index)+1));

  this->_high_index = high_index;
  this->_low_index = low_index;
}


void vcSlice::Print(ostream& ofile)
{
  ofile << vcLexerKeywords[__SLICE_OP] << " " << this->Get_Label() << " "
	<< vcLexerKeywords[__LPAREN] 
	<< this->_din->Get_Id() << " "
	<< this->_high_index << " "
	<< this->_low_index << " "
	<< vcLexerKeywords[__RPAREN] 
	<< " "
	<< vcLexerKeywords[__LPAREN] 
	<< this->_dout->Get_Id()
	<< vcLexerKeywords[__RPAREN] 
	<< " ";
  this->Print_Guard(ofile);
	this->Print_Flow_Through(ofile);
  ofile << endl;
  this->Print_Delay(ofile);
}

void vcSlice::Print_VHDL(ostream& ofile)
{
        string inst_name = this->Get_VHDL_Id();
	string block_name = inst_name + "_block"; 
        string name = '"' + inst_name + '"';
	bool flow_through = this->Get_Flow_Through();

	ofile << block_name << " : block -- { " << endl;
	ofile << "signal sample_req, sample_ack, update_req, update_ack: BooleanArray(0 downto 0); " << endl;
	if(!flow_through && (this->_guard_wire != NULL))
	{
		ofile << " signal sample_req_ug, sample_ack_ug, update_req_ug, update_ack_ug: BooleanArray(0 downto 0); " << endl;
		ofile << " signal guard_vector : std_logic_vector(0 downto 0); " << endl;

		string buf_const;
		string gflags;
		vector<vcDatapathElement*> ops;
		vector<vcWire*> gwires;	
		
		ops.push_back(this);
		gwires.push_back(this->_guard_wire);
		Generate_Guard_Constants(buf_const, gflags, ops, gwires);

		ofile << buf_const << endl;
		ofile << gflags << endl;
	}
	ofile << " -- } " << endl;
      ofile << "begin -- { " << endl;
 

      if(!flow_through and (this->Get_Guard_Wire() != NULL))
      {
        	ofile << " sample_req_ug(0) <= "   << this->Get_Req(0)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(0)->Get_DP_To_CP_Symbol() << "<= sample_ack_ug(0);" << endl;
        	ofile << " update_req_ug(0) <= "   << this->Get_Req(1)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(1)->Get_DP_To_CP_Symbol() << "<= update_ack_ug(0);" << endl;
		ofile << " guard_vector(0) <= " << this->_guard_wire->Get_VHDL_Signal_Id() << "(0);" << endl; 
      }
      else
      {
        	ofile << " sample_req(0) <= "   << this->Get_Req(0)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(0)->Get_DP_To_CP_Symbol() << "<= sample_ack(0);" << endl;
        	ofile << " update_req(0) <= "   << this->Get_Req(1)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(1)->Get_DP_To_CP_Symbol() << "<= update_ack(0);" << endl;
      }

      int ip_buf = this->Get_Input_Buffering(this->_din);
      int op_buf = this->Get_Output_Buffering(this->_dout);
      int buf_size = ((ip_buf < op_buf) ? op_buf : ip_buf);

      if(!flow_through && (this->_guard_wire != NULL))
      {
	      string guards = "guard_vector";
	      vector<vcWire*> gwires;
	      gwires.push_back(this->_guard_wire);
	      Print_VHDL_Guard_Instance("gI", 1, "guardBuffering", "guardFlags", "guard_vector",
			      "sample_req_ug", "sample_ack_ug", "sample_req", "sample_ack",
			      "update_req_ug", "update_ack_ug", "update_req", "update_ack", ofile);
      }

      ofile << this->Get_VHDL_Id() << ": SliceSplitProtocol generic map(name => " << name
	      << ", in_data_width => " << this->_din->Get_Size() 
	      << ", high_index => " << this->_high_index 
	      << ", low_index => " << this->_low_index  << "," 
	      << " buffering => " << buf_size << ","
	      << " flow_through => " << (flow_through ? "true" : "false") 
	      << ") -- {" << endl;
      ofile << " port map( din => " 
	      << this->_din->Get_VHDL_Signal_Id() 
	      << ", dout => " 
	      << this->_dout->Get_VHDL_Signal_Id() 
	      << ", sample_req => sample_req(0) " 
	      << ", sample_ack => sample_ack(0) "
	      << ", update_req => update_req(0) " 
	      << ", update_ack => update_ack(0) "
	      << ", clk => clk, reset => reset); -- }" << endl;
      ofile << "-- }" << endl << "end block;" << endl;
}


  	
vcPermutation::vcPermutation(string id, vcWire* din, vcWire* dout, vector<pair<int,int> >& bmapv):vcSplitOperator(id)
{
  this->_din = din;
  _din->Connect_Receiver(this);

  this->_dout = dout;
  _dout->Connect_Driver(this);

  for(int idx = 0, fidx = bmapv.size(); idx < fidx; idx++)
  {
	int hi = bmapv[idx].first;
	int li = bmapv[idx].second;

	if((hi >= 0) && (li >= 0) && (hi < din->Get_Size()) && (li < din->Get_Size()))
		_permutation.push_back(pair<int,int>(bmapv[idx].first, bmapv[idx].second));
  }
}

void vcPermutation::Print(ostream& ofile)
{
	ofile << vcLexerKeywords[__PERMUTE_OP] << " " << this->Get_Label() << " "
		<< vcLexerKeywords[__LPAREN] 
		<< this->_din->Get_Id() << " ";
	for(int idx = 0, fidx = _permutation.size(); idx < fidx; idx++)
	{
		ofile <<  _permutation[idx].first << " " << _permutation[idx].second << " ";
	}
	ofile << vcLexerKeywords[__RPAREN]  << " ";
	ofile << vcLexerKeywords[__LPAREN] 
		<< this->_dout->Get_Id()
		<< " "
		<< vcLexerKeywords[__RPAREN] 
		<< " ";
	this->Print_Guard(ofile);
	this->Print_Flow_Through(ofile);
	ofile << endl;
	this->Print_Delay(ofile);
}


void vcPermutation::Print_VHDL(ostream& ofile)
{
	//
	// do the swizzling, and instantiate an interlock element.
	//
        string inst_name = this->Get_VHDL_Id();
	string block_name = inst_name + "_block"; 
        string name = '"' + inst_name + '"';
	bool flow_through = this->Get_Flow_Through();

	ofile << block_name << " : block -- { " << endl;
	ofile << "signal sample_req, sample_ack, update_req, update_ack: BooleanArray(0 downto 0); " << endl;
	if(!flow_through && (this->_guard_wire != NULL))
	{
		ofile << " signal sample_req_ug, sample_ack_ug, update_req_ug, update_ack_ug: BooleanArray(0 downto 0); " << endl;
		ofile << " signal guard_vector : std_logic_vector(0 downto 0); " << endl;

		string buf_const;
		string gflags;
		vector<vcDatapathElement*> ops;
		vector<vcWire*> gwires;	
		
		ops.push_back(this);
		gwires.push_back(this->_guard_wire);
		Generate_Guard_Constants(buf_const, gflags, ops, gwires);

		ofile << buf_const << endl;
		ofile << gflags << endl;
	}
	// print intermediate wire..
	ofile << " signal permuted_sig: std_logic_vector(" << this->_din->Get_Size()-1
		<< " downto 0);" << endl;
	ofile << " -- } " << endl;
      ofile << "begin -- { " << endl;
 

      if(!flow_through and (this->Get_Guard_Wire() != NULL))
      {
        	ofile << " sample_req_ug(0) <= "   << this->Get_Req(0)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(0)->Get_DP_To_CP_Symbol() << "<= sample_ack_ug(0);" << endl;
        	ofile << " update_req_ug(0) <= "   << this->Get_Req(1)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(1)->Get_DP_To_CP_Symbol() << "<= update_ack_ug(0);" << endl;
		ofile << " guard_vector(0) <= " << this->_guard_wire->Get_VHDL_Signal_Id() << "(0);" << endl; 
      }
      else
      {
        	ofile << " sample_req(0) <= "   << this->Get_Req(0)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(0)->Get_DP_To_CP_Symbol() << "<= sample_ack(0);" << endl;
        	ofile << " update_req(0) <= "   << this->Get_Req(1)->Get_CP_To_DP_Symbol() << ";" << endl;
        	ofile <<  this->Get_Ack(1)->Get_DP_To_CP_Symbol() << "<= update_ack(0);" << endl;
      }

      int ip_buf = this->Get_Input_Buffering(this->_din);
      int op_buf = this->Get_Output_Buffering(this->_dout);
      int buf_size = ((ip_buf < op_buf) ? op_buf : ip_buf);

      if(!flow_through && (this->_guard_wire != NULL))
      {
	      string guards = "guard_vector";
	      vector<vcWire*> gwires;
	      gwires.push_back(this->_guard_wire);
	      Print_VHDL_Guard_Instance("gI", 1, "guardBuffering", "guardFlags", "guard_vector",
			      "sample_req_ug", "sample_ack_ug", "sample_req", "sample_ack",
			      "update_req_ug", "update_ack_ug", "update_req", "update_ack", ofile);
      }

      ofile << " -- permutation " << endl;
      set<int> mapped_dests;
      set<int> mapped_srcs;
      string src_net = this->_din->Get_VHDL_Id();
      for(int idx = 0, fidx = this->_permutation.size(); idx < fidx; idx++)
	{
		int s = this->_permutation[idx].first;
		int d  = this->_permutation[idx].second;

		mapped_dests.insert(d);
		mapped_srcs.insert(s);

		ofile << " permuted_sig(" << d << ") <= " << src_net << "(" << s << "); " << endl;
	}
      ofile << " -- unmapped target bits.. " << endl;
      for(int idx = 0, fidx = this->_din->Get_Size(); idx < fidx; idx++)
	{
		if(mapped_dests.find(idx) == mapped_dests.end())
		{
			if(mapped_srcs.find(idx) == mapped_srcs.end())
				ofile << " permuted_sig(" << idx << ") <= " << src_net << "(" << idx << "); " << endl;
			else
				vcSystem::Error("inconsistent permutation specification of operator " + 
							inst_name);
		}
	}
     
      ofile <<  "ilb: InterlockBuffer generic map(name => " << name
	      	<< ", in_data_width => " << this->_din->Get_Size() 
	      	<< ", out_data_width => " << this->_dout->Get_Size() 
	      	<< ", buffer_size => " << buf_size 
	      	<< ", flow_through => " << (flow_through ? "true" : "false") 
	      << ") -- {" << endl;
      ofile << " port map( write_data => permuted_sig" 
	      << ", read_data => " 
	      << this->_dout->Get_VHDL_Signal_Id() 
	      << ", write_req => sample_req(0) " 
	      << ", write_ack => sample_ack(0) "
	      << ", read_req => update_req(0) " 
	      << ", read_ack => update_ack(0) "
	      << ", clk => clk, reset => reset); -- }" << endl;
      ofile << "-- }" << endl << "end block;" << endl;
}



vcRegister::vcRegister(string id, vcWire* din, vcWire* dout):vcOperator(id)
{
	assert(din && dout);

	_din = din;
	din->Connect_Receiver(this);

	_dout = dout;
	dout->Connect_Driver(this);
}

void vcRegister::Print(ostream& ofile)
{
	ofile << vcLexerKeywords[__ASSIGN_OP] << " " << this->Get_Label() << " "
		<< vcLexerKeywords[__LPAREN] 
		<< this->_din->Get_Id() << " "
		<< vcLexerKeywords[__RPAREN] 
		<< " "
		<< vcLexerKeywords[__LPAREN] 
		<< this->_dout->Get_Id()
		<< vcLexerKeywords[__RPAREN] 
		<< " ";
	this->Print_Guard(ofile);
	ofile << endl;
	this->Print_Delay(ofile);
}

void vcRegister::Print_VHDL(ostream& ofile)
{
	// NOTE: the register is now obsolete, replaced by the InterlockBuffer.
	assert(0);
	string block_name = "register_" + this->Get_VHDL_Id() + "_wrap";
	ofile << block_name << " : block -- { " << endl;
	if(this->Get_Guard_Wire() != NULL)
		ofile << "signal ack_no_guard: boolean;" << endl;
	ofile << "signal req, ack: boolean; --}" << endl;
	ofile << "begin -- { " << endl;


	if(this->Get_Guard_Wire() != NULL)
	{
		ofile << "req <= " << this->Get_Req(0)->Get_CP_To_DP_Symbol() 
			<< " when " << this->Get_Guard_Wire()->Get_VHDL_Id() 
			<< "(0) = " << (this->Get_Guard_Complement() ? "'0'"  : "'1'") << " else false;" << endl; 
		ofile << " ackDelay: control_delay_element generic map(delay_value => 1) "
			<< " port map(req => " 
			<< this->Get_Req(0)->Get_CP_To_DP_Symbol() 
			<< ", ack => ack_no_guard, clk => clk, reset => reset);" << endl;
		ofile << this->Get_Ack(0)->Get_DP_To_CP_Symbol() << " <= ack " 
			<< " when " << this->Get_Guard_Wire()->Get_VHDL_Id() 
			<< "(0) = " << (this->Get_Guard_Complement() ? "'0'"  : "'1'") << " else " 
			<< " ack_no_guard "  << ";" 
			<< endl; 
	}
	else
	{
		ofile << "req <= " << this->Get_Req(0)->Get_CP_To_DP_Symbol() << ";" << endl;
		ofile << this->Get_Ack(0)->Get_DP_To_CP_Symbol() << " <= ack; "  << endl;
	}

	ofile << this->Get_VHDL_Id() << ": RegisterBase --{" << endl
		<< "generic map(in_data_width => " << this->_din->Get_Size()  << "," 
		<< "out_data_width => " << this->_dout->Get_Size() << ") "
		<< endl;
	ofile << " port map( din => " << this->_din->Get_VHDL_Signal_Id() << "," 
		<< " dout => " << this->_dout->Get_VHDL_Signal_Id() << ","
		<< " req => req, " 
		<< " ack => ack, " << " clk => clk, reset => reset); -- }" << endl;
	ofile << "-- }" << endl << "end block;" << endl;
}

vcBranch::vcBranch(string id, vector<vcWire*>& wires): vcDatapathElement(id) 
{
	for(int idx = 0; idx < wires.size(); idx++)
	{
		_inwires.push_back(wires[idx]);
		wires[idx]->Connect_Receiver(this);
	}
}

void vcBranch::Add_Reqs(vector<vcTransition*>& reqs)
{
	assert(reqs.size() == 1);
	this->_reqs = reqs;
}

void vcBranch::Add_Acks(vector<vcTransition*>& acks)
{
	assert(acks.size() == 2);
	this->_acks = acks;
}

void vcBranch::Print(ostream& ofile)
{
	ofile << vcLexerKeywords[__BRANCH_OP] << " " << this->Get_Label() << " " 
		<< vcLexerKeywords[__LPAREN];
	for(int idx =0; idx < _inwires.size(); idx++)
	{
		if(idx > 0)
			ofile << " ";
		ofile << _inwires[idx]->Get_Id();
	}
	ofile << vcLexerKeywords[__RPAREN] << endl;
	this->Print_Delay(ofile);
}


bool Is_Shift_Op(string vc_op_id)
{
	bool ret_val = false;
	if(vc_op_id == vcLexerKeywords[__SHL_OP]                ) { ret_val = true;} 
	else if(vc_op_id == vcLexerKeywords[__SHR_OP]           ) { ret_val = true;} 
	else if(vc_op_id == vcLexerKeywords[__SHRA_OP]          ) { ret_val = true;} 
	else if(vc_op_id == vcLexerKeywords[__ROL_OP]          ) { ret_val = true;} 
	else if(vc_op_id == vcLexerKeywords[__ROR_OP]          ) { ret_val = true;} 
	return(ret_val);
}

bool vcBinarySplitOperator::Is_Int_Add_Op()
{
	if(this->Get_Input_Type()->Is_Int_Type())
	{
		if(this->_op_id == vcLexerKeywords[__PLUS_OP] || 
				this->_op_id == vcLexerKeywords[__MINUS_OP])
			return(true);
		else
			return(false);
	}
	else
		return(false);
}

bool Is_Compare_Op(string vc_op_id)
{
	bool ret_val = false;
	if(vc_op_id == vcLexerKeywords[__SGT_OP]        ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__SGE_OP]        ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__EQ_OP]        ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__SLT_OP]        ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__SLE_OP]        ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__UGT_OP]        ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__UGE_OP]        ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__ULT_OP]        ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__ULE_OP]        ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__NEQ_OP]        ) { ret_val = true; }

	return(ret_val);
}

bool Is_Trivial_Op(string vc_op_id)
{
	bool ret_val = false;
	if(vc_op_id == vcLexerKeywords[__BITSEL_OP]             ) { ret_val = true;      } 
	else if(vc_op_id == vcLexerKeywords[__CONCAT_OP]        ) { ret_val = true; } 
	else if(vc_op_id == vcLexerKeywords[__SLICE_OP]        ) { ret_val = true; } 
	else if(vc_op_id == vcLexerKeywords[__PERMUTE_OP]       ) { ret_val = true; } 
	else if(vc_op_id == vcLexerKeywords[__OR_OP]            ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__AND_OP]           ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__XOR_OP]           ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__NOR_OP]           ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__NAND_OP]          ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__XNOR_OP]          ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__ASSIGN_OP]        ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__NOT_OP]        ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__UtoS_ASSIGN_OP]   ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__StoU_ASSIGN_OP]   ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__StoS_ASSIGN_OP]   ) { ret_val = true; }
	return(ret_val);
}

bool Is_Symmetric_Op(string vc_op_id)
{
	bool ret_val = false;
	if(vc_op_id == vcLexerKeywords[__PLUS_OP]               )      { ret_val = true;      } 
	else if(vc_op_id == vcLexerKeywords[__MUL_OP]           ) { ret_val = true; } 
	else if(vc_op_id == vcLexerKeywords[__EQ_OP]            ) { ret_val = true; } 
	else if(vc_op_id == vcLexerKeywords[__OR_OP]            ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__AND_OP]           ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__XOR_OP]           ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__NOR_OP]           ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__NAND_OP]          ) { ret_val = true; }
	else if(vc_op_id == vcLexerKeywords[__XNOR_OP]          ) { ret_val = true; }
	return(ret_val);
}

bool Is_Unary_Op(string vc_op_id)
{
	return((vc_op_id == vcLexerKeywords[__NOT_OP]) ||
			(vc_op_id == vcLexerKeywords[__ASSIGN_OP]) ||
			(vc_op_id == vcLexerKeywords[__StoS_ASSIGN_OP]) ||
			(vc_op_id == vcLexerKeywords[__UtoS_ASSIGN_OP]) ||
			(vc_op_id == vcLexerKeywords[__StoU_ASSIGN_OP]) ||
			(vc_op_id == vcLexerKeywords[__FtoS_ASSIGN_OP]) ||
			(vc_op_id == vcLexerKeywords[__FtoU_ASSIGN_OP]) ||
			(vc_op_id == vcLexerKeywords[__UtoF_ASSIGN_OP]) ||
			(vc_op_id == vcLexerKeywords[__StoF_ASSIGN_OP]) ||
			(vc_op_id == vcLexerKeywords[__FtoF_ASSIGN_OP]));
}

string Get_VHDL_Op_Id(string vc_op_id, vcType* in_type, vcType* out_type)
{

	string ret_string;

	if((in_type->Kind() == "vcIntType" || in_type->Kind() == "vcPointerType") &&
			(out_type->Kind() == "vcIntType" || out_type->Kind() == "vcPointerType"))
	{
		if(vc_op_id == vcLexerKeywords[__PLUS_OP]          ) { ret_string = "ApIntAdd";} 
		else if(vc_op_id == vcLexerKeywords[__MINUS_OP]         ) { ret_string = "ApIntSub";} 
		else if(vc_op_id == vcLexerKeywords[__MUL_OP]           ) { ret_string = "ApIntMul";} 
		else if(vc_op_id == vcLexerKeywords[__DIV_OP]           ) { ret_string = "ApIntDiv";} 
		else if(vc_op_id == vcLexerKeywords[__SHL_OP]           ) { ret_string = "ApIntSHL";} 
		else if(vc_op_id == vcLexerKeywords[__SHR_OP]           ) { ret_string = "ApIntLSHR";} 
		else if(vc_op_id == vcLexerKeywords[__SHRA_OP]          ) { ret_string = "ApIntASHR";} 
		else if(vc_op_id == vcLexerKeywords[__ROL_OP]           ) { ret_string = "ApIntROL";} 
		else if(vc_op_id == vcLexerKeywords[__ROR_OP]           ) { ret_string = "ApIntROR";} 
		else if(vc_op_id == vcLexerKeywords[__SGT_OP]            ) { ret_string = "ApIntSgt";} 
		else if(vc_op_id == vcLexerKeywords[__SGE_OP]            ) { ret_string = "ApIntSge"  ;} 
		else if(vc_op_id == vcLexerKeywords[__EQ_OP]            ) { ret_string = "ApIntEq"  ;} 
		else if(vc_op_id == vcLexerKeywords[__SLT_OP]            ) { ret_string = "ApIntSlt"  ;} 
		else if(vc_op_id == vcLexerKeywords[__SLE_OP]            ) { ret_string = "ApIntSle"  ;}
		else if(vc_op_id == vcLexerKeywords[__UGT_OP]           ) { ret_string = "ApIntUgt"  ;}
		else if(vc_op_id == vcLexerKeywords[__UGE_OP]           ) { ret_string = "ApIntUge"  ;}
		else if(vc_op_id == vcLexerKeywords[__ULT_OP]           ) { ret_string = "ApIntUlt"  ;}
		else if(vc_op_id == vcLexerKeywords[__ULE_OP]           ) { ret_string = "ApIntUle"  ;}
		else if(vc_op_id == vcLexerKeywords[__NEQ_OP]           ) { ret_string = "ApIntNe"  ;} 
		else if(vc_op_id == vcLexerKeywords[__BITSEL_OP]        ) { ret_string = "ApBitsel"  ;} 
		else if(vc_op_id == vcLexerKeywords[__CONCAT_OP]        ) { ret_string = "ApConcat"  ;} 
		else if(vc_op_id == vcLexerKeywords[__ASSIGN_OP]        ) { ret_string = "ApIntToApIntUnsigned" ;}
		else if(vc_op_id == vcLexerKeywords[__StoS_ASSIGN_OP]   ) { ret_string = "ApIntToApIntSigned" ;}
		else if(vc_op_id == vcLexerKeywords[__UtoS_ASSIGN_OP]   ) { ret_string = "ApIntToApIntSigned" ;}
		else if(vc_op_id == vcLexerKeywords[__StoU_ASSIGN_OP]   ) { ret_string = "ApIntToApIntUnsigned" ;}
		else if(vc_op_id == vcLexerKeywords[__NOT_OP]           ) { ret_string = "ApIntNot"  ;}
		else if(vc_op_id == vcLexerKeywords[__OR_OP]            ) { ret_string = "ApIntOr"  ;}
		else if(vc_op_id == vcLexerKeywords[__AND_OP]           ) { ret_string = "ApIntAnd"  ;}
		else if(vc_op_id == vcLexerKeywords[__XOR_OP]           ) { ret_string = "ApIntXor"  ;}
		else if(vc_op_id == vcLexerKeywords[__NOR_OP]           ) { ret_string = "ApIntNor"  ;}
		else if(vc_op_id == vcLexerKeywords[__NAND_OP]          ) { ret_string = "ApIntNand"  ;}
		else if(vc_op_id == vcLexerKeywords[__XNOR_OP]          ) { ret_string = "ApIntXnor"  ;}
		else { vcSystem::Error("unsupported int X int -> int operation " + vc_op_id);}
	}

	if(in_type->Kind() == "vcFloatType" && out_type->Kind() == "vcFloatType")
	{
		if(vc_op_id == vcLexerKeywords[__PLUS_OP]               ) { ret_string = "ApFloatAdd";} 
		else if(vc_op_id == vcLexerKeywords[__MINUS_OP]         ) { ret_string = "ApFloatSub";} 
		else if(vc_op_id == vcLexerKeywords[__MUL_OP]           ) { ret_string = "ApFloatMul";} 
		else if(vc_op_id == vcLexerKeywords[__DIV_OP]           ) { ret_string = "ApFloatDiv";} 
		else if(vc_op_id == vcLexerKeywords[__ASSIGN_OP]        ) { ret_string = "ApFloatResize";}
		else if(vc_op_id == vcLexerKeywords[__FtoF_ASSIGN_OP]   ) { ret_string = "ApFloatResize";}
		else { vcSystem::Error("unsupported float <-> float operation " + vc_op_id);}
	}

	if(in_type->Kind() == "vcFloatType" && out_type->Kind() == "vcIntType")
	{
		// all float comparison operations will be unordered.. (no exception handling!)
		if(vc_op_id == vcLexerKeywords[__SGT_OP]            ) { ret_string = "ApFloatUgt" ;} 
		else if(vc_op_id == vcLexerKeywords[__SGE_OP]            ) { ret_string = "ApFloatUge"  ;} 
		else if(vc_op_id == vcLexerKeywords[__EQ_OP]            ) { ret_string = "ApFloatUeq"  ;} 
		else if(vc_op_id == vcLexerKeywords[__SLT_OP]            ) { ret_string = "ApFloatUlt"  ;} 
		else if(vc_op_id == vcLexerKeywords[__SLE_OP]            ) { ret_string = "ApFloatUle"  ;}
		else if(vc_op_id == vcLexerKeywords[__NEQ_OP]           ) { ret_string = "ApFloatUne"  ;} 
		else if(vc_op_id == vcLexerKeywords[__BITSEL_OP]        ) { ret_string = "ApBitsel"   ;} 
		else if(vc_op_id == vcLexerKeywords[__CONCAT_OP]        ) { ret_string = "ApConcat"   ;} 
		else if(vc_op_id == vcLexerKeywords[__FtoS_ASSIGN_OP]        ) { ret_string = "ApFloatToApIntSigned";}
		else if(vc_op_id == vcLexerKeywords[__FtoU_ASSIGN_OP]        ) { ret_string = "ApFloatToApIntUnsigned";}

		else { vcSystem::Error("unsupported float <-> int operation " + vc_op_id);}
	}
	if((in_type->Is("vcIntType") || in_type->Is("vcPointerType")) && out_type->Kind() == "vcFloatType")
	{
		if(vc_op_id == vcLexerKeywords[__ASSIGN_OP] ) { ret_string = "ApIntToApFloatUnsigned";}
		else if(vc_op_id == vcLexerKeywords[__StoF_ASSIGN_OP]        ) { ret_string = "ApIntToApFloatSigned";}
		else if(vc_op_id == vcLexerKeywords[__UtoF_ASSIGN_OP]        ) 
		{ 
			ret_string = "ApIntToApFloatUnsigned";
		}      
		else { vcSystem::Error("unsupported int -> float operation " + vc_op_id);}
	}

	string q_ret_string = '"' + ret_string + '"';
	return(q_ret_string);
}

bool Is_Pipelined_Float_Op(string vc_op_id, vcType* in_type, vcType* out_type, int& exponent_width, int& fraction_width)
{
	bool ret_val = false;
	if(in_type->Kind() == "vcFloatType" && out_type->Kind() == "vcFloatType")
	{
		int e1 = ((vcFloatType*)in_type)->Get_Characteristic_Width();
		int f1 = ((vcFloatType*)in_type)->Get_Mantissa_Width();

		int e2 = ((vcFloatType*)out_type)->Get_Characteristic_Width();
		int f2 = ((vcFloatType*)out_type)->Get_Mantissa_Width();

		if((e1 == e2) && (f1 == f2))
		{
			exponent_width = e1;
			fraction_width = f1;

			if( ((e1 == 8) && (f1 == 23)) || ((e1 == 11) and (f1 == 52)))
			{
				if(vc_op_id == vcLexerKeywords[__PLUS_OP]               ) { ret_val = true;}
				else if(vc_op_id == vcLexerKeywords[__MINUS_OP]         ) { ret_val = true;}
				else if(vc_op_id == vcLexerKeywords[__MUL_OP]           ) { ret_val = true;}
				else
					ret_val = false;
			}
		}
	}
	return(ret_val);
}


bool Check_If_Equivalent(vector<vcWire*>& iw1, vector<vcWire*>& iw2)
{
	bool ret_val = true;

	if(iw1.size() != iw2.size())
		return(false);

	for(int idx = 0; idx < iw1.size(); idx++)
	{
		vcWire* w1 = iw1[idx];
		vcWire* w2 = iw2[idx];

		if(w1->Get_Type()->Is_Int_Type() != w2->Get_Type()->Is_Int_Type())
		{
			ret_val = false;
			break;
		}

		if(w1->Is("vcConstantWire")  != w2->Is("vcConstantWire"))
		{
			ret_val = false;
			break;
		}

		if(w1->Get_Type()->Size() != w2->Get_Type()->Size())
		{
			ret_val = false;
			break;
		}

		if(!w1->Get_Type()->Is_Int_Type() && (w1->Get_Type() != w2->Get_Type()))
		{
			ret_val = false;
			break;
		}
	}

	return(ret_val);
}

void vcInterlockBuffer::Add_Reqs(vector<vcTransition*>& reqs)
{
	assert (reqs.size() == 2);
	_reqs = reqs;
}

void vcInterlockBuffer::Add_Acks(vector<vcTransition*>& acks)
{
	assert (acks.size() == 2);
	_acks = acks;
}

void vcInterlockBuffer::Print_VHDL(ostream& ofile)
{
	string inst_name = this->Get_VHDL_Id();
	string block_name = inst_name + "_block"; 
	string name = '"' + inst_name + '"';

	ofile << block_name << ": block -- {" << endl;
	ofile << " signal wreq, wack, rreq, rack: BooleanArray(0 downto 0); " << endl;
	if(this->_guard_wire != NULL)
	{
		ofile << " signal wreq_ug, wack_ug, rreq_ug, rack_ug: BooleanArray(0 downto 0); " << endl;
		ofile << " signal guard_vector : std_logic_vector(0 downto 0); " << endl;

		string buf_const;
		string gflags;
		vector<vcDatapathElement*> ops;
		vector<vcWire*> gwires;	

		ops.push_back(this);
		gwires.push_back(this->_guard_wire);
		Generate_Guard_Constants(buf_const, gflags, ops, gwires);

		ofile << buf_const << endl;
		ofile << gflags << endl;
	}
	ofile << " -- } " << endl;
	ofile << "begin -- {" << endl;

	if(this->_guard_wire != NULL)
	{
		ofile << " wreq_ug(0) <= "   << this->Get_Req(0)->Get_CP_To_DP_Symbol() << ";" << endl;
		ofile <<  this->Get_Ack(0)->Get_DP_To_CP_Symbol() << "<= wack_ug(0);" << endl;
		ofile << " rreq_ug(0) <= "   << this->Get_Req(1)->Get_CP_To_DP_Symbol() << ";" << endl;
		ofile <<  this->Get_Ack(1)->Get_DP_To_CP_Symbol() << "<= rack_ug(0);" << endl;
		ofile << " guard_vector(0) <= " << this->_guard_wire->Get_VHDL_Signal_Id() << "(0);" << endl; 
	}
	else
	{
		ofile << " wreq(0) <= "   << this->Get_Req(0)->Get_CP_To_DP_Symbol() << ";" << endl;
		ofile <<  this->Get_Ack(0)->Get_DP_To_CP_Symbol() << "<= wack(0);" << endl;
		ofile << " rreq(0) <= "   << this->Get_Req(1)->Get_CP_To_DP_Symbol() << ";" << endl;
		ofile <<  this->Get_Ack(1)->Get_DP_To_CP_Symbol() << "<= rack(0);" << endl;
	}

	int ip_buf = this->Get_Input_Buffering(this->_din);
	int op_buf = this->Get_Output_Buffering(this->_dout);
	int buf_size = ((ip_buf < op_buf) ? op_buf : ip_buf);

	if(this->_guard_wire != NULL)
	{
		string guards = "guard_vector";
		vector<vcWire*> gwires;
		gwires.push_back(this->_guard_wire);
		Print_VHDL_Guard_Instance("gI", 1, "guardBuffering", "guardFlags", "guard_vector",
				"wreq_ug", "wack_ug", "wreq", "wack",
				"rreq_ug", "rack_ug", "rreq", "rack", ofile);
	}

	int in_data_width = this->_din->Get_Size();
	int out_data_width = this->_dout->Get_Size();
        bool flow_through = this->Get_Flow_Through();

	ofile << this->Get_VHDL_Id() << " : InterlockBuffer ";
	ofile << "generic map ( -- { " << endl;
	ofile << " name => " << name << "," << endl;
	ofile << " buffer_size => " << buf_size << "," << endl;
 	ofile << " flow_through => " <<  (flow_through ? " true " : " false ") << "," << endl;
	ofile << " in_data_width => " << in_data_width << "," << endl;
	ofile << " out_data_width => " << out_data_width <<  endl;
	ofile << " -- }" << endl << ")";
	ofile << "port map ( -- { " << endl;
	ofile << " write_req => wreq(0), "   << endl;
	ofile << " write_ack => wack(0), "   << endl;
	ofile << " write_data => "  << this->_din->Get_VHDL_Signal_Id() << "," << endl;
	ofile << " read_req => rreq(0),  "   << endl;
	ofile << " read_ack => rack(0), "   << endl;
	ofile << " read_data => "  << this->_dout->Get_VHDL_Signal_Id() << "," << endl;
	ofile << " clk => clk, reset => reset" << endl;
	ofile << " -- }" << endl << ");" << endl;
	ofile << "end block; -- } " << endl;
}

void vcInterlockBuffer::Print(ostream& ofile)
{
	ofile << vcLexerKeywords[__HASH] << " ";
	ofile << vcLexerKeywords[__ASSIGN_OP] << " " << this->Get_Label() << " "
		<< vcLexerKeywords[__LPAREN] 
		<< this->_din->Get_Id() << " "
		<< vcLexerKeywords[__RPAREN] 
		<< " "
		<< vcLexerKeywords[__LPAREN] 
		<< this->_dout->Get_Id()
		<< vcLexerKeywords[__RPAREN] 
		<< " ";
	this->Print_Guard(ofile);
	this->Print_Flow_Through(ofile);
	ofile << endl;
	this->Print_Delay(ofile);
}


