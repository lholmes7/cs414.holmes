(* Simple strategies *)

let quicksort lst =
  List.sort compare lst

let rec mergesort lst =
  match lst with
  | [] -> []
  | [x] -> [x]
  | _ ->
    (* split list in two halves *)
    let rec split a b = function
      | [] -> (List.rev a, List.rev b)
      | x :: xs -> split b (x :: a) xs
    in
    let left, right = split [] [] lst in

    (* merge lists *)
    let rec merge l r =
      match l, r with
      | [], _ -> r
      | _, [] -> l
      | x::xs, y::ys ->
        if x <= y then x :: merge xs r
        else y :: merge l ys
    in

    merge (mergesort left) (mergesort right)

let bubblesort lst =
  let rec pass lst =
    match lst with
    | a :: b :: rest ->
      if a > b then b :: pass (a :: rest)
      else a :: pass (b :: rest)
    | _ -> lst
  in
  let rec loop lst =
    let next = pass lst in
    if next = lst then lst
    else loop next
  in
  loop lst

(* Simple context function *)

let sort_with strategy lst =
  strategy lst


(* Example (simple version) *)

let () =
  let data = [5; 2; 9; 1; 5; 6] in
  let result = sort_with quicksort data in
  Printf.printf "Simple / quicksort: ";
  List.iter (Printf.printf "%d ") result;
  print_newline ();



module type SORT = sig
  val sort : int list -> int list
end

module Quick : SORT = struct
  let sort = List.sort compare
end

module Merge : SORT = struct
  let rec sort lst =
    match lst with
    | [] -> []
    | [x] -> [x]
    | _ ->
      let rec split a b = function
        | [] -> (List.rev a, List.rev b)
        | x :: xs -> split b (x :: a) xs
      in
      let left, right = split [] [] lst in

      let rec merge l r =
        match l, r with
        | [], _ -> r
        | _, [] -> l
        | x::xs, y::ys ->
          if x <= y then x :: merge xs r
          else y :: merge l ys
      in
      merge (sort left) (sort right)
end

module Bubble : SORT = struct
  let rec sort lst =
    let rec pass lst =
      match lst with
      | a :: b :: rest ->
        if a > b then b :: pass (a :: rest)
        else a :: pass (b :: rest)
      | _ -> lst
    in
    let next = pass lst in
    if next = lst then lst
    else sort next
end

module SortContext (S : SORT) = struct
  let execute_strategy lst = S.sort lst
end

module CQuick  = SortContext(Quick)
module CMerge  = SortContext(Merge)
module CBubble = SortContext(Bubble)


(* Example (functor version) *)

let () =
  let data = [5; 2; 9; 1; 5; 6] in

  let r1 = CQuick.execute_strategy data in
  let r2 = CMerge.execute_strategy data in
  let r3 = CBubble.execute_strategy data in

  let print name lst =
    Printf.printf "%s: " name;
    List.iter (Printf.printf "%d ") lst;
    print_newline ()
  in

  print "Functor / quicksort" r1;
  print "Functor / mergesort" r2;
  print "Functor / bubblesort" r3;
  print_newline ();
  ()
