(*
  * File:   main.ml
  * Author: Your Glorious Instructor
  * Purpose:
  * Demo code for a binary tree type.
  *)

type avl_tree =
  | Empty
  | Node of {
      value : int;
      left : avl_tree;
      right : avl_tree;
      height : int;
    }

let height = function
  | Empty -> 0
  | Node { height; _ } -> height

let make_node v l r =
  let h = 1 + max (height l) (height r) in
  Node { value = v; left = l; right = r; height = h }

let balance_factor = function
  | Empty -> 0
  | Node { left; right; _ } -> height left - height right

let rotate_right = function
  | Node { value = y; left = Node { value = x; left = a; right = b; _ }; right = c; _ } ->
      make_node x a (make_node y b c)
  | t -> t  (* should not happen in balanced insert *)

let rotate_left = function
  | Node { value = x; left = a; right = Node { value = y; left = b; right = c; _ }; _ } ->
      make_node y (make_node x a b) c
  | t -> t

let balance t =
  match t with
  | Empty -> Empty
  | Node _ as node ->
      let bf = balance_factor node in
      if bf > 1 then
        (* Left-heavy *)
        if balance_factor (match node with Node { left; _ } -> left | _ -> Empty) < 0 then
          (* Left-Right case *)
          match node with
          | Node { value; left; right; _ } ->
              make_node value (rotate_left left) right |> rotate_right
          | _ -> node
        else
          rotate_right node
      else if bf < -1 then
        (* Right-heavy *)
        if balance_factor (match node with Node { right; _ } -> right | _ -> Empty) > 0 then
          (* Right-Left case *)
          match node with
          | Node { value; left; right; _ } ->
              make_node value left (rotate_right right) |> rotate_left
          | _ -> node
        else
          rotate_left node
      else
        node

let rec insert t x =
  match t with
  | Empty -> make_node x Empty Empty
  | Node { value; left; right; _ } as node ->
      if x < value then
        balance (make_node value (insert left x) right)
      else if x > value then
        balance (make_node value left (insert right x))
      else
        node  (* duplicates not allowed *)


let rec string_of_avl t =
  match t with
  | Empty -> "()"
  | Node { value; left; right; height } ->
      "Node{value=" ^ string_of_int value
      ^ "; height=" ^ string_of_int height
      ^ "; left=" ^ string_of_avl left
      ^ "; right=" ^ string_of_avl right
      ^ "}"
