



`include "Fetch_Cycle.v"
`include "Decode_Cyle.v"
`include "Execute_Cycle.v"
`include "Memory_Cycle.v"
`include "Writeback_Cycle.v"

`include "BTB.v"

`include "PC.v"
`include "PC_Adder.v"
`include "Mux.v"
`include "Instruction_Memory.v"
`include "Control_Unit_Top.v"
`include "Register_File.v"
`include "Sign_Extend.v"
`include "ALU.v"
`include "Data_Memory.v"
`include "Hazard_unit.v"

module Pipeline_top(clk, rst);


    input clk, rst;


    wire PCSrcE;

    wire RegWriteW;

    wire RegWriteE;
    wire ALUSrcE;
    wire MemWriteE;
    wire ResultSrcE;
    wire BranchE;

    wire RegWriteM;
    wire MemWriteM;
    wire ResultSrcM;
    wire ResultSrcW;

    wire [2:0] ALUControlE;

    wire [4:0] RD_E;
    wire [4:0] RD_M;
    wire [4:0] RDW;

    wire [31:0] PCTargetE;

    wire [31:0] InstrD;
    wire [31:0] PCD;
    wire [31:0] PCPlus4D;

    wire [31:0] ResultW;

    wire [31:0] RD1_E;
    wire [31:0] RD2_E;
    wire [31:0] Imm_Ext_E;

    wire [31:0] PCE;
    wire [31:0] PCPlus4E;

    wire [31:0] PCPlus4M;
    wire [31:0] WriteDataM;
    wire [31:0] ALU_ResultM;

    wire [31:0] PCPlus4W;
    wire [31:0] ALU_ResultW;
    wire [31:0] ReadDataW;

    wire [4:0] RS1_E;
    wire [4:0] RS2_E;

    wire [1:0] ForwardBE;
    wire [1:0] ForwardAE;


    wire btb_hit;

    wire predict_taken;

    wire [31:0] predicted_target;

    wire branch_taken_E;

    wire mispredict_E;

    wire [31:0] correct_pc_E;


    BTB btb_unit(

        .clk(clk),
        .reset(~rst),

        .pc_fetch(PCD),

        .hit(btb_hit),

        .predicted_target(predicted_target),

        .predict_taken(predict_taken),

        .update(branch_taken_E),

        .pc_update(PCE),

        .actual_target(PCTargetE),

        .actual_taken(branch_taken_E)

    );


    fetch_cycle Fetch (

                        .clk(clk),

                        .rst(rst),

                        .PCSrcE(PCSrcE),

                        .PCTargetE(correct_pc_E),
                        .flush(mispredict_E),
                        .predict_taken(predict_taken),

                        .predicted_target(predicted_target),

                        .InstrD(InstrD),

                        .PCD(PCD),

                        .PCPlus4D(PCPlus4D)

                    );


    decode_cycle Decode (

                        .clk(clk),

                        .rst(rst),

                        .flush(mispredict_E),
                        
                        .InstrD(InstrD),

                        .PCD(PCD),

                        .PCPlus4D(PCPlus4D),

                        .RegWriteW(RegWriteW),

                        .RDW(RDW),

                        .ResultW(ResultW),

                        .RegWriteE(RegWriteE),

                        .ALUSrcE(ALUSrcE),

                        .MemWriteE(MemWriteE),

                        .ResultSrcE(ResultSrcE),

                        .BranchE(BranchE),

                        .ALUControlE(ALUControlE),

                        .RD1_E(RD1_E),

                        .RD2_E(RD2_E),

                        .Imm_Ext_E(Imm_Ext_E),

                        .RD_E(RD_E),

                        .PCE(PCE),

                        .PCPlus4E(PCPlus4E),

                        .RS1_E(RS1_E),

                        .RS2_E(RS2_E)

                    );


    execute_cycle Execute (

                        .clk(clk),

                        .rst(rst),

                        .RegWriteE(RegWriteE),

                        .ALUSrcE(ALUSrcE),

                        .MemWriteE(MemWriteE),

                        .ResultSrcE(ResultSrcE),

                        .BranchE(BranchE),

                        .ALUControlE(ALUControlE),

                        .RD1_E(RD1_E),

                        .RD2_E(RD2_E),

                        .Imm_Ext_E(Imm_Ext_E),

                        .RD_E(RD_E),

                        .PCE(PCE),

                        .PCPlus4E(PCPlus4E),

                        .predict_taken_E(predict_taken),

                        .PCSrcE(PCSrcE),

                        .PCTargetE(PCTargetE),

                        .branch_taken_E(branch_taken_E),

                        .mispredict_E(mispredict_E),

                        .correct_pc_E(correct_pc_E),

                        .RegWriteM(RegWriteM),

                        .MemWriteM(MemWriteM),

                        .ResultSrcM(ResultSrcM),

                        .RD_M(RD_M),

                        .PCPlus4M(PCPlus4M),

                        .WriteDataM(WriteDataM),

                        .ALU_ResultM(ALU_ResultM),

                        .ResultW(ResultW),

                        .ForwardA_E(ForwardAE),

                        .ForwardB_E(ForwardBE)

                    );


    memory_cycle Memory (

                        .clk(clk),

                        .rst(rst),

                        .RegWriteM(RegWriteM),

                        .MemWriteM(MemWriteM),

                        .ResultSrcM(ResultSrcM),

                        .RD_M(RD_M),

                        .PCPlus4M(PCPlus4M),

                        .WriteDataM(WriteDataM),

                        .ALU_ResultM(ALU_ResultM),

                        .RegWriteW(RegWriteW),

                        .ResultSrcW(ResultSrcW),

                        .RD_W(RDW),

                        .PCPlus4W(PCPlus4W),

                        .ALU_ResultW(ALU_ResultW),

                        .ReadDataW(ReadDataW)

                    );


    writeback_cycle WriteBack (

                        .clk(clk),

                        .rst(rst),

                        .ResultSrcW(ResultSrcW),

                        .PCPlus4W(PCPlus4W),

                        .ALU_ResultW(ALU_ResultW),

                        .ReadDataW(ReadDataW),

                        .ResultW(ResultW)

                    );


    hazard_unit Forwarding_block (

                        .rst(rst),

                        .RegWriteM(RegWriteM),

                        .RegWriteW(RegWriteW),

                        .RD_M(RD_M),

                        .RD_W(RDW),

                        .Rs1_E(RS1_E),

                        .Rs2_E(RS2_E),

                        .ForwardAE(ForwardAE),

                        .ForwardBE(ForwardBE)

                        );

endmodule