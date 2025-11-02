(* ---------------- Option---------------- *)
module OptionM = struct
  type 'a t = 'a option
  let return x = Some x
  let bind m f = match m with
    | None -> None
    | Some x -> f x
  let map f m = match m with
    | None -> None
    | Some x -> Some (f x)
end

(* Global operator for OptionM tests *)
let ( >>= ) m f = OptionM.bind m f
let ( >|= ) m f = OptionM.map f m

(* ---------------- Result---------------- *)
module ResultM = struct
  type 'a t = ('a, string) result
  let return x = Ok x
  let bind m f = match m with
    | Error e -> Error e
    | Ok x -> f x
  let map f m = match m with
    | Error e -> Error e
    | Ok x -> Ok (f x)
end

(* Global operator for ResultM *)
let ( >>=! ) m f = ResultM.bind m f
let ( >|=! ) m f = ResultM.map f m

(* ---------------- List---------------- *)
module ListM = struct
  type 'a t = 'a list
  let return x = [x]
  let bind xs f = List.concat (List.map f xs)
  let map f xs = List.map f xs
end

(* Global operator for ListM *)
let ( >>== ) xs f = ListM.bind xs f
let ( >|== ) xs f = ListM.map f xs

(* ----------------- tests Option----------------- *)
let safe_div x y =
  if y = 0 then None else Some (x / y)

let test_option () =
  let result =
    safe_div 12 3 >>= fun x ->
    safe_div x 2 >>= fun y ->
    OptionM.return (y + 1)
  in
  match result with
  | None -> print_endline "Option test: None"
  | Some v -> Printf.printf "Option test: Some %d\n" v

(* ----------------- tests Result----------------- *)
let safe_div_r x y =
  if y = 0 then Error "divide by zero" else Ok (x / y)

let test_result () =
  let result =
    safe_div_r 12 3 >>=! fun x ->
    safe_div_r x 2 >>=! fun y ->
    ResultM.return (y + 1)
  in
  match result with
  | Error e -> Printf.printf "Result test: Error %s\n" e
  | Ok v -> Printf.printf "Result test: Ok %d\n" v

(* ----------------- tests List----------------- *)
let test_list () =
  let xs = [1;2;3] in
  let result =
    xs >>== fun x ->
    [x; x*2]
  in
  Printf.printf "List test: [%s]\n"
    (String.concat "; " (List.map string_of_int result))

(* ----------------- Run all tests ----------------- *)
let () =
  test_option ();
  test_result ();
  test_list ()
