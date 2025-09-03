type nat = 
  | Z
  | S of nat

let rec to_int = function
  | Z -> 0
  | S n -> 1 + to_int n
let rec add x y =
  match x with
  | Z -> y
  | S x_prev -> S (add x_prev y)
let rec mul x y =
  match x with
  | Z -> Z
  | S x_prev -> add y (mul x_prev y)
let rec sub x y =
  match (x, y) with
  | (n, Z) -> n
  | (Z, _) -> Z
  | (S x_prev, S y_prev) -> sub x_prev y_prev
let rec div x y =
  match (x, y) with
  | (_, Z) -> failwith "Division by zero"
  | (Z, _) -> Z
  | _ ->
      let rest = sub x y in
      match rest with
      | Z -> S Z  (* exactly divisible once *)
      | _ when rest = x -> Z  (* y > x *)
      | _ -> S (div rest y)
(* Main test *)
let one = S Z
let two = S one
let three = S two
let four = S three

let () =
  print_endline ("1 + 4 = " ^ string_of_int (to_int (add one four)));
  print_endline ("2 * 2 = " ^ string_of_int (to_int (mul two two)));
  print_endline ("4 / 2 = " ^ string_of_int (to_int (div four two)));
  print_endline ("1 / 1 = " ^ string_of_int (to_int (div one one)));