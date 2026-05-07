`include "Pipeline_Top.v"
`timescale 1ns/1ps

module tb();


    reg clk = 0;

    reg rst;


    always begin

        clk = ~clk;

        #50;

    end


    Pipeline_top dut (

                    .clk(clk),

                    .rst(rst)

                    );


    initial begin


        rst = 1'b0;

        #200;


        rst = 1'b1;


        #5000;

        $finish;

    end


    initial begin

        $dumpfile("pipeline.vcd");

        $dumpvars(0, tb);

    end


    initial begin

        $monitor(

        "TIME=%0t | PC=%h | predict_taken=%b | branch_taken=%b | mispredict=%b | target=%h",

        $time,

        dut.PCD,

        dut.predict_taken,

        dut.branch_taken_E,

        dut.mispredict_E,

        dut.PCTargetE

        );

    end

endmodule