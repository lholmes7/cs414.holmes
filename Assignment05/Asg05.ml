type 'a zipper = {
  left_rev : 'a list;  (* Prefocus *)
  focus : 'a option;   (* focus *)
  right : 'a list;     (* postfocus *)
}

let empty () = { left_rev = []; focus = None; right = [] }

let is_empty z =
  z.focus = None && z.left_rev = [] && z.right = []

let move_left z =
  match z.left_rev, z.focus with
  | [], _ -> z
  | x :: xs, _ ->
      { left_rev = xs; focus = Some x; right = (match z.focus with Some v -> v :: z.right | None -> z.right) }

let move_right z =
  match z.focus, z.right with
  | _, [] -> z
  | Some v, x :: xs -> { left_rev = v :: z.left_rev; focus = Some x; right = xs }
  | None, x :: xs -> { left_rev = z.left_rev; focus = Some x; right = xs }

let push_front x z = { z with left_rev = x :: z.left_rev }
let push_back x z = { z with right = z.right @ [x] }
let focus z = z.focus

(* print zipper*)
let print_zipper z =
  let left = List.rev z.left_rev |> List.map string_of_int |> String.concat ", " in
  let right = z.right |> List.map string_of_int |> String.concat ", " in
  let f = match z.focus with Some x -> string_of_int x | None -> "None" in
  Printf.printf "Left: [%s], Focus: %s, Right: [%s]\n" left f right

(* Test *)
let () =
  let z = empty () in
  let z = push_back 1 z |> push_back 2 |> push_back 3 |> push_back 4 in
  Printf.printf "Initial zipper:\n"; print_zipper z;

  let z = move_right z in
  Printf.printf "After move_right 1:\n"; print_zipper z;

  let z = move_right z in
  Printf.printf "After move_right 2:\n"; print_zipper z;

  let z = move_left z in
  Printf.printf "After move_left 1:\n"; print_zipper z;

  let z = push_front 0 z in
  Printf.printf "After push_front 0:\n"; print_zipper z;

  let z = push_back 5 z in
  Printf.printf "After push_back 5:\n"; print_zipper z;

  match focus z with
  | Some x -> Printf.printf "Current focus: %d\n" x
  | None -> print_endline "No focus"
