



module fetch_cycle(

    clk,
    rst,

    PCSrcE,
    PCTargetE,

    flush,

    predict_taken,
    predicted_target,

    InstrD,
    PCD,
    PCPlus4D

);


    input clk;
    input rst;

    input PCSrcE;

    input [31:0] PCTargetE;

    input flush;

    input predict_taken;

    input [31:0] predicted_target;


    output [31:0] InstrD;

    output [31:0] PCD;
    output [31:0] PCPlus4D;


    wire [31:0] PC_F;

    wire [31:0] PCF;

    wire [31:0] PCPlus4F;

    wire [31:0] InstrF;

    wire [31:0] PredictedPC;


    reg [31:0] InstrF_reg;

    reg [31:0] PCF_reg;

    reg [31:0] PCPlus4F_reg;


    PC_Adder PC_adder (

                .a(PCF),

                .b(32'h00000004),

                .c(PCPlus4F)

                );


    assign PredictedPC =
            (predict_taken) ? predicted_target : PCPlus4F;


    Mux PC_MUX (

                .a(PredictedPC),

                .b(PCTargetE),

                .s(PCSrcE),

                .c(PC_F)

                );


    PC_Module Program_Counter (

                .clk(clk),

                .rst(rst),

                .PC(PCF),

                .PC_Next(PC_F)

                );


    Instruction_Memory IMEM (

                .rst(rst),

                .A(PCF),

                .RD(InstrF)

                );


    always @(posedge clk or negedge rst) begin


        if(rst == 1'b0) begin

            InstrF_reg <= 32'h00000000;

            PCF_reg <= 32'h00000000;

            PCPlus4F_reg <= 32'h00000000;

        end


        else if(flush) begin


            InstrF_reg <= 32'h00000013;

            PCF_reg <= 32'h00000000;

            PCPlus4F_reg <= 32'h00000000;

        end


        else begin

            InstrF_reg <= InstrF;

            PCF_reg <= PCF;

            PCPlus4F_reg <= PCPlus4F;

        end
    end


    assign InstrD =
            (rst == 1'b0) ? 32'h00000000 : InstrF_reg;

    assign PCD =
            (rst == 1'b0) ? 32'h00000000 : PCF_reg;

    assign PCPlus4D =
            (rst == 1'b0) ? 32'h00000000 : PCPlus4F_reg;

endmodule