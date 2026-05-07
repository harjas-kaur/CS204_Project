



module decode_cycle(

    clk,
    rst,

    flush,

    InstrD,
    PCD,
    PCPlus4D,

    RegWriteW,
    RDW,
    ResultW,

    RegWriteE,
    ALUSrcE,
    MemWriteE,
    ResultSrcE,
    BranchE,

    ALUControlE,

    RD1_E,
    RD2_E,
    Imm_Ext_E,

    RD_E,

    PCE,
    PCPlus4E,

    RS1_E,
    RS2_E

);


    input clk;
    input rst;

    input flush;

    input RegWriteW;

    input [4:0] RDW;

    input [31:0] InstrD;
    input [31:0] PCD;
    input [31:0] PCPlus4D;
    input [31:0] ResultW;

    output RegWriteE;
    output ALUSrcE;
    output MemWriteE;
    output ResultSrcE;
    output BranchE;

    output [2:0] ALUControlE;

    output [31:0] RD1_E;
    output [31:0] RD2_E;
    output [31:0] Imm_Ext_E;

    output [4:0] RS1_E;
    output [4:0] RS2_E;
    output [4:0] RD_E;

    output [31:0] PCE;
    output [31:0] PCPlus4E;


    wire RegWriteD;
    wire ALUSrcD;
    wire MemWriteD;
    wire ResultSrcD;
    wire BranchD;

    wire [1:0] ImmSrcD;

    wire [2:0] ALUControlD;

    wire [31:0] RD1_D;
    wire [31:0] RD2_D;
    wire [31:0] Imm_Ext_D;


    reg RegWriteD_r;
    reg ALUSrcD_r;
    reg MemWriteD_r;
    reg ResultSrcD_r;
    reg BranchD_r;

    reg [2:0] ALUControlD_r;

    reg [31:0] RD1_D_r;
    reg [31:0] RD2_D_r;
    reg [31:0] Imm_Ext_D_r;

    reg [4:0] RD_D_r;
    reg [4:0] RS1_D_r;
    reg [4:0] RS2_D_r;

    reg [31:0] PCD_r;
    reg [31:0] PCPlus4D_r;


    Control_Unit_Top control (

                            .Op(InstrD[6:0]),

                            .RegWrite(RegWriteD),

                            .ImmSrc(ImmSrcD),

                            .ALUSrc(ALUSrcD),

                            .MemWrite(MemWriteD),

                            .ResultSrc(ResultSrcD),

                            .Branch(BranchD),

                            .funct3(InstrD[14:12]),

                            .funct7(InstrD[31:25]),

                            .ALUControl(ALUControlD)

                            );


    Register_File rf (

                        .clk(clk),

                        .rst(rst),

                        .WE3(RegWriteW),

                        .WD3(ResultW),

                        .A1(InstrD[19:15]),

                        .A2(InstrD[24:20]),

                        .A3(RDW),

                        .RD1(RD1_D),

                        .RD2(RD2_D)

                        );


    Sign_Extend extension (

                        .In(InstrD[31:0]),

                        .Imm_Ext(Imm_Ext_D),

                        .ImmSrc(ImmSrcD)

                        );


    always @(posedge clk or negedge rst) begin

        if(rst == 1'b0) begin

            RegWriteD_r <= 1'b0;

            ALUSrcD_r <= 1'b0;

            MemWriteD_r <= 1'b0;

            ResultSrcD_r <= 1'b0;

            BranchD_r <= 1'b0;

            ALUControlD_r <= 3'b000;

            RD1_D_r <= 32'h00000000;

            RD2_D_r <= 32'h00000000;

            Imm_Ext_D_r <= 32'h00000000;

            RD_D_r <= 5'h00;

            PCD_r <= 32'h00000000;

            PCPlus4D_r <= 32'h00000000;

            RS1_D_r <= 5'h00;

            RS2_D_r <= 5'h00;

        end


        else if(flush) begin


            RegWriteD_r <= 1'b0;

            ALUSrcD_r <= 1'b0;

            MemWriteD_r <= 1'b0;

            ResultSrcD_r <= 1'b0;

            BranchD_r <= 1'b0;

            ALUControlD_r <= 3'b000;

            RD1_D_r <= 32'h00000000;

            RD2_D_r <= 32'h00000000;

            Imm_Ext_D_r <= 32'h00000000;

            RD_D_r <= 5'h00;

            PCD_r <= 32'h00000000;

            PCPlus4D_r <= 32'h00000000;

            RS1_D_r <= 5'h00;

            RS2_D_r <= 5'h00;

        end


        else begin

            RegWriteD_r <= RegWriteD;

            ALUSrcD_r <= ALUSrcD;

            MemWriteD_r <= MemWriteD;

            ResultSrcD_r <= ResultSrcD;

            BranchD_r <= BranchD;

            ALUControlD_r <= ALUControlD;

            RD1_D_r <= RD1_D;

            RD2_D_r <= RD2_D;

            Imm_Ext_D_r <= Imm_Ext_D;

            RD_D_r <= InstrD[11:7];

            PCD_r <= PCD;

            PCPlus4D_r <= PCPlus4D;

            RS1_D_r <= InstrD[19:15];

            RS2_D_r <= InstrD[24:20];

        end
    end


    assign RegWriteE = RegWriteD_r;

    assign ALUSrcE = ALUSrcD_r;

    assign MemWriteE = MemWriteD_r;

    assign ResultSrcE = ResultSrcD_r;

    assign BranchE = BranchD_r;

    assign ALUControlE = ALUControlD_r;

    assign RD1_E = RD1_D_r;

    assign RD2_E = RD2_D_r;

    assign Imm_Ext_E = Imm_Ext_D_r;

    assign RD_E = RD_D_r;

    assign PCE = PCD_r;

    assign PCPlus4E = PCPlus4D_r;

    assign RS1_E = RS1_D_r;

    assign RS2_E = RS2_D_r;

endmodule