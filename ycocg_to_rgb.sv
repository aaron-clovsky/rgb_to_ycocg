function [7:0] clamp_u8;
    input signed [9:0] value;
    begin
        clamp_u8 = value[9] ? 8'h00 : value[8] ? 8'hFF : value[7:0];
    end
endfunction

function automatic void ycocg_to_rgb;
    input [7:0] Y, Co, Cg;
    output [7:0] R, G, B;
    begin
        logic signed [9:0] s_Y = 10'($signed({2'b0, Y}));
        logic signed [9:0] s_Co = 10'($signed(Co));
        logic signed [9:0] s_Cg = 10'($signed(Cg));
        logic signed [9:0] tmp = s_Y - s_Cg;
        
        R = clamp_u8(tmp + s_Co);
        G = clamp_u8(s_Y + s_Cg);
        B = clamp_u8(tmp - s_Co);
    end
endfunction

module ycocg_to_rgb_test;

    logic [7:0] R, G, B;
    
    initial begin
        for (int Y = 0; Y <= 255; Y++) begin
            for (int Co = 0; Co <= 255; Co++) begin
                for (int Cg = 0; Cg <= 255; Cg++) begin
                    ycocg_to_rgb(Y[7:0], Co[7:0], Cg[7:0], R, G, B);
                    
                    $display("%0d,%0d,%0d,%0d,%0d,%0d", Y, Co, Cg, R, G, B);
                end
            end
        end
        $finish;
    end

endmodule
