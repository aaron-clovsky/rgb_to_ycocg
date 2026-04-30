module ycocg_to_rgb (
    input  [7:0] Y,
    input  [7:0] Co,
    input  [7:0] Cg,
    output [7:0] R,
    output [7:0] G,
    output [7:0] B
);
    function [7:0] clamp;
        input signed [9:0] value;
        begin
            clamp = value[9] ? 8'h00 : value[8] ? 8'hFF : value[7:0];
        end
    endfunction

    wire signed [9:0] s_Y  = $signed({2'b0, Y});
    wire signed [9:0] s_Co = $signed(Co);
    wire signed [9:0] s_Cg = $signed(Cg);
    wire signed [9:0] s_tmp = s_Y - s_Cg;

    wire signed [9:0] out_r = s_tmp + s_Co;
    wire signed [9:0] out_g = s_Y + s_Cg;
    wire signed [9:0] out_b = s_tmp - s_Co;

    assign R = clamp(out_r);
    assign G = clamp(out_g);
    assign B = clamp(out_b);

endmodule
