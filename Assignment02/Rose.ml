(* 
* File: Rose.ml 
* Author: Landon Holmes
* Assignment2.1 *)

type 'a rose = Node of 'a * 'a rose list

(* Count number of nodes in the rose tree *)
let rec size (Node (_, children)) =
  1 + List.fold_left (fun acc c -> acc + size c) 0 children

(* Map a function over all values in the tree *)
let rec map f (Node (v, children)) =
  Node (f v, List.map (map f) children)

(* General fold over the rose tree *)
let rec fold f (Node (v, children)) =
  f v (List.map (fold f) children)

(* ---------- Example usage ---------- *)
let example_tree =
  Node (1, [
    Node (2, []);
    Node (3, [Node (4, []); Node (5, [])])
  ])

let () =
  Printf.printf "Size = %d\n" (size example_tree);

  let doubled = map (fun x -> x * 2) example_tree in
  let sum = fold (fun v cs -> v + List.fold_left (+) 0 cs) example_tree in

  Printf.printf "Sum = %d\n" sum;
  match doubled with
  | Node (v, _) -> Printf.printf "Root of doubled tree = %d\n" v