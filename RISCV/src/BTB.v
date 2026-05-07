module BTB(

    input clk,
    input reset,

    input [31:0] pc_fetch,

    output reg hit,
    output reg [31:0] predicted_target,
    output reg predict_taken,

    input update,

    input [31:0] pc_update,

    input [31:0] actual_target,

    input actual_taken

);


    integer i;

    reg valid [0:15];

    reg [27:0] tag [0:15];

    reg [31:0] target [0:15];


    reg [1:0] predictor [0:15];


    wire [3:0] fetch_index;

    wire [3:0] update_index;

    assign fetch_index = pc_fetch[5:2];

    assign update_index = pc_update[5:2];


    always @(*) begin

        if(valid[fetch_index] &&
           tag[fetch_index] == pc_fetch[31:4]) begin

            hit = 1'b1;

            predicted_target = target[fetch_index];

            predict_taken = predictor[fetch_index][1];

        end

        else begin

            hit = 1'b0;

            predicted_target = 32'b0;

            predict_taken = 1'b0;

        end
    end


    always @(posedge clk or posedge reset) begin

        if(reset) begin

            for(i = 0; i < 16; i = i + 1) begin

                valid[i] <= 1'b0;

                tag[i] <= 28'b0;

                target[i] <= 32'b0;

                predictor[i] <= 2'b01;

            end
        end

        else if(update) begin

            valid[update_index] <= 1'b1;

            tag[update_index] <= pc_update[31:4];

            target[update_index] <= actual_target;


            case(predictor[update_index])

                2'b00:
                    predictor[update_index]
                        <= actual_taken ? 2'b01 : 2'b00;

                2'b01:
                    predictor[update_index]
                        <= actual_taken ? 2'b10 : 2'b00;

                2'b10:
                    predictor[update_index]
                        <= actual_taken ? 2'b11 : 2'b01;

                2'b11:
                    predictor[update_index]
                        <= actual_taken ? 2'b11 : 2'b10;

            endcase

        end
    end

endmodule